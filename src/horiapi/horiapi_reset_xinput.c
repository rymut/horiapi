#include <horiapi/horiapi.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <Windows.h>
#include <initguid.h> 
#include <devpkey.h>
#include <devguid.h>
#include <SetupAPI.h>
#include <Cfgmgr32.h>

#include <hidapi/hidapi.h>


#pragma comment (lib, "Setupapi")
#pragma comment(lib, "Cfgmgr32")
#pragma comment(lib, "Hid")

#include "hori_device.h"
#include "hori_device_win32.h"
#include "hori_context.h"

wchar_t* hori_internal_get_device_interfaces_string_list(wchar_t* device_id, LPGUID interface_class_guid);
wchar_t* hori_device_win32_get_hid_location_path(const char* hid_path, wchar_t** device_instance);

static uint8_t hori_internal_get_firmware_command[] = { 15, 0, 0, 60, 9 };

static uint8_t hori_internal_heartbeat_command[] = { 15, 170, 85, 90, 165 };

/**
     * @brief Write heartbeat to device
     * @param[in] device - device to write heartbeat
     */
int hori_send_heartbeat(hori_device_t* hori_dev)
{
    if (hori_dev == NULL) {
        return -1;
    }
    if (hori_dev->control == NULL) {
        return -1;
    }
    int write = hid_write(hori_dev->control, hori_internal_heartbeat_command, sizeof(hori_internal_heartbeat_command));
    return write;
}

// FORM FEAD 'A' 'P' 'P' 
//static uint8_t hori_internal_enter_config_command_steam[] = { 0, 5, 12, 65, 80, 80 };
static uint8_t hori_internal_enter_config_command_ps5[] = { 2, 12, 65, 80, 80 };
static uint8_t hori_internal_enter_config_command_ps4[] = { 5, 12, 65, 80, 80 };

int hori_internal_is_xinput(hori_device_t* device) {
    if (device == NULL) {
        return 0;
    }
    return device->control == NULL;
}
int hori_internal_send_enter_config_xinput(hori_device_t* device) {
    GUID xboxClassGuid = { 0xec87f1e3, 0xc13b, 0x4100, { 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb } };
    wchar_t* list = NULL;
    ULONG len = 0;
    wchar_t* device_id = NULL;
    wchar_t* locationPath = hori_device_win32_get_hid_location_path(hid_get_device_info(device->gamepad)->path, &device_id);
    free(locationPath);

    if (device_id == NULL) {
        return -1;
    }

    struct hori_internal_string_list device_interfaces;
    memset(&device_interfaces, 0, sizeof(device_interfaces));
    device_interfaces.value = hori_internal_get_device_interfaces_string_list(device_id, &xboxClassGuid);
    free(device_id);

    device_interfaces.byte_size = hori_internal_string_list_size(device_interfaces.value, &device_interfaces.count);
    if (device_interfaces.count < 1) {
        free(device_interfaces.value);
        return -1;
    }
    //query_controller(list);
    set_rumble_sequence(device_interfaces.value);
    return 0;
}

int hori_internal_send_enter_config_hid(hori_device_t* device) {
    if (device->gamepad == NULL) {
        return -1;
    }
    int sum = 0;
    sum += hid_write(device->gamepad, hori_internal_enter_config_command_ps4, sizeof(hori_internal_enter_config_command_ps4));
    sum += hid_write(device->gamepad, hori_internal_enter_config_command_ps5, sizeof(hori_internal_enter_config_command_ps5));
    if (sum > 0) {
        return sum + 1;
    }
    return -1;
}

int hori_internal_send_enter_config(hori_device_t* device) {
    if (hori_internal_is_xinput(device)) {
        return hori_internal_send_enter_config_xinput(device);
    }
    else {
        return hori_internal_send_enter_config_hid(device);
    }
}

enum {
    device_interface_success = 0,
    device_interface_error = 1,
};

DEVPROPTYPE get_device_instance_property_type(DEVINST deviceInstance, const DEVPROPKEY* propertyKey) {
    if (propertyKey == NULL) {
        return DEVPROP_TYPE_EMPTY;
    }
    DEVPROPTYPE type = DEVPROP_TYPE_NULL;
    ULONG size = 0;
    CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, NULL, &size, 0);
    return type;
}

ULONG get_device_instance_property_size(DEVINST deviceInstance, const DEVPROPKEY* propertyKey) {
    if (propertyKey == NULL) {
        return 0;
    }
    DEVPROPTYPE type = DEVPROP_TYPE_NULL;
    ULONG size = 0;
    CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, NULL, &size, 0);
    return size;
}

DEVPROPTYPE get_device_interface_property_type(LPCWSTR path, const DEVPROPKEY* prop) {
    if (prop == NULL) {
        return 0;
    }
    ULONG size = 0;
    DEVPROPTYPE type = 0;
    if (CR_BUFFER_SMALL != CM_Get_Device_Interface_PropertyW(path, prop, &type, NULL, &size, 0)) {
        return 0;
    }
    return type;
}

ULONG get_device_interface_property_size(LPCWSTR path, const DEVPROPKEY* prop) {
    if (prop == NULL) {
        return 0;
    }
    ULONG size = 0;
    DEVPROPTYPE type = 0;
    if (CR_BUFFER_SMALL != CM_Get_Device_Interface_PropertyW(path, prop, &type, NULL, &size, 0)) {
        return 0;
    }
    return size;
}

static BOOL get_device_interface_property_guid(LPCWSTR path, const DEVPROPKEY* prop, GUID* value) {
    GUID result;
    GUID* buffer = &result;
    const ULONG buffer_size = sizeof(GUID);
    const DEVPROPTYPE buffer_type = DEVPROP_TYPE_GUID;
    if (value) {
        buffer = value;
    }
    return get_device_interface_property(path, prop, buffer_type, buffer, buffer_size);
}



HANDLE OpenDeviceInterface(const wchar_t* path, BOOL readOnly)
{
    DWORD desired_access = readOnly == TRUE ? 0 : (GENERIC_WRITE | GENERIC_READ);
    DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    HANDLE handle = CreateFileW(path, desired_access, share_mode, 0, OPEN_EXISTING, 0, 0);

    return handle;
}
#include <inttypes.h>

wchar_t* hori_internal_get_device_interfaces_string_list(wchar_t* device_id, LPGUID interface_class_guid)
{
    ULONG device_id_size = 0;
    /*
    wchar_t device_id[MAX_DEVICE_ID_LEN];
    memset(device_id, 0, sizeof(device_id));

    if (CR_SUCCESS != CM_Get_Device_IDW(device_instance, device_id, sizeof(device_id) / sizeof(device_id[0]), 0)) {
        return NULL;
    }
    */
    ULONG string_list_element_count = 0;
    if (CR_SUCCESS != CM_Get_Device_Interface_List_SizeW(&string_list_element_count, interface_class_guid, device_id, CM_GET_DEVICE_INTERFACE_LIST_PRESENT) || string_list_element_count <= 1) {
        return NULL;
    }

    wchar_t* string_list = (wchar_t*)calloc(string_list_element_count, sizeof(wchar_t));
    if (string_list == NULL) {
        return NULL;
    }
    if (CR_SUCCESS != CM_Get_Device_Interface_ListW(interface_class_guid, device_id, string_list, string_list_element_count, CM_GET_DEVICE_INTERFACE_LIST_PRESENT)) {
        free(string_list);
        return NULL;
    }

    return string_list;
}

// https://gist.github.com/mmozeiko/b8ccc54037a5eaf35432396feabbe435
int GetDevInstInterfaces(DEVINST dev, LPGUID interfaceGUID, wchar_t** outIfaces, ULONG* outIfacesLen)
{
    CONFIGRET cres;
    if (!outIfaces)
        return -1;
    if (!outIfacesLen)
        return -2;

    // Get System Device ID
    WCHAR sysDeviceID[256];

    cres = CM_Get_Device_IDW(dev, sysDeviceID, sizeof(sysDeviceID) / sizeof(sysDeviceID[0]), 0);
    if (cres != CR_SUCCESS)
        return -11;

    // Get list size
    ULONG ifaceListSize = 0;
    cres = CM_Get_Device_Interface_List_SizeW(&ifaceListSize, interfaceGUID, sysDeviceID, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cres != CR_SUCCESS)
        return -12;

    // Allocate memory for the list
    wchar_t* ifaceList = (wchar_t*)calloc(ifaceListSize, sizeof(wchar_t));

    // Populate the list
    cres = CM_Get_Device_Interface_ListW(interfaceGUID, sysDeviceID, ifaceList, ifaceListSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cres != CR_SUCCESS) {
        free(ifaceList);
        return -13;
    }

    // Return list
    *outIfaces = ifaceList;
    *outIfacesLen = ifaceListSize;

    return 0;
}

BOOL query_controller(const wchar_t* path) {
    if (path == NULL)
        return FALSE;

    HANDLE handle = OpenDeviceInterface(path, FALSE);

    BY_HANDLE_FILE_INFORMATION  result;
    if (handle == INVALID_HANDLE_VALUE)
        return FALSE;

    uint8_t gamepadStateRequest0101[3] = { 0x01, 0x01, 0x00 };
    uint8_t ledStateData[3] = { 0, 0,0 };
    DWORD len = 0;

    // https://github.com/nefarius/XInputHooker/issues/1
    // https://gist.github.com/mmozeiko/b8ccc54037a5eaf35432396feabbe435
    DWORD IOCTL_XUSB_GET_LED_STATE = 0x8000E008;

    if (!DeviceIoControl(handle,
        IOCTL_XUSB_GET_LED_STATE,
        gamepadStateRequest0101,
        3,
        ledStateData,
        3,
        &len,
        NULL))
    {
        // GetLastError()
        return FALSE;
    }


    //DCHECK_EQ(len, ledStateData.size());

    // https://www.partsnotincluded.com/xbox-360-controller-led-animations-info/
    // https://github.com/paroj/xpad/blob/5978d1020344c3288701ef70ea9a54dfc3312733/xpad.c#L1382-L1402
    uint8_t kInvalidXInputUserId = 0xff;
    uint8_t XINPUT_LED_TO_PORT_MAP[] =
    {
        kInvalidXInputUserId,   // All off
        kInvalidXInputUserId,   // All blinking, then previous setting
        0,                      // 1 flashes, then on
        1,                      // 2 flashes, then on
        2,                      // 3 flashes, then on
        3,                      // 4 flashes, then on
        0,                      // 1 on
        1,                      // 2 on
        2,                      // 3 on
        3,                      // 4 on
        kInvalidXInputUserId,   // Rotate
        kInvalidXInputUserId,   // Blink, based on previous setting
        kInvalidXInputUserId,   // Slow blink, based on previous setting
        kInvalidXInputUserId,   // Rotate with two lights
        kInvalidXInputUserId,   // Persistent slow all blink
        kInvalidXInputUserId,   // Blink once, then previous setting
    };

    const uint8_t ledState = ledStateData[2];
    printf("led state is %d\n", ledState);
    //DCHECK_LE(ledState, std::size(XINPUT_LED_TO_PORT_MAP));

    uint8_t m_XInputUserIndex = XINPUT_LED_TO_PORT_MAP[ledState];

    return TRUE;
}

int set_rumble(HANDLE handle, uint16_t low, uint16_t high)
{
    if (handle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    //BYTE rumble_packet[] = { 0x00, 0x08, low >> 8, high >> 8, 0x00, 0x00, 0x00, 0x00 };
    BYTE in[5] = { 0, 0, low >> 8 , high >> 8, 2 };
    if (!DeviceIoControl(handle, 0x8000a010, in, sizeof(in), NULL, 0, NULL, NULL))
        //if (!DeviceIoControl(handle, 0x8000a010, rumble_packet, sizeof(rumble_packet), NULL, 0, NULL, NULL))
    {
        // NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
        return -1;
    }
    return 0;
}

int set_rumble_sequence(const wchar_t* path) {
    DWORD delay = 5, time = 15;
    DWORD sleep[] = { delay, time, delay, time, delay, time, delay, time, delay };
    WORD values[] = { 0, 13311, 0, 6655, 0, 13311, 0, 32767, 0 };
    const size_t steps = sizeof(sleep) / sizeof(DWORD);
    HANDLE handle = OpenDeviceInterface(path, FALSE);
    if (handle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    for (size_t i = 0; i < steps; ++i) {
        if (-1 == set_rumble(handle, values[i], values[i])) {
            CloseHandle(handle);
            return -1;
        }
        Sleep(sleep[i]);
    }
    CloseHandle(handle);
    return 0;
}


int hori_internal_string_list_size(wchar_t* string_list, int* size) {
    wchar_t* ptr = string_list;
    int element_count = 0;
    int item_count = 0;
    if (ptr != NULL) {
        for (int len = wcslen(ptr); len != 0; ) {
            ptr = ptr + len + 1;
            item_count++;
            element_count += len + 1;
            len = wcslen(ptr);
        }
    }
    if (size != NULL) {
        *size = item_count;
    }
    return (element_count + 1) * sizeof(wchar_t);
}

hori_device_t* hori_open(hori_product_t product, int index, hori_context_t* context) {
    hori_enumeration_t* devs = hori_enumerate(product, context);

    hori_free_enumerate(devs);
    return NULL;
}

void hori_internal_hid_close(hori_device_t* device) {
    if (device == NULL) {
        return;
    }
    hid_close(device->control);
    device->control = NULL;
    hid_close(device->gamepad);
    device->gamepad = NULL;
}

int hori_internal_leave_config(hori_device_t* device) {
    return 0;
}


int hori_internal_open(hori_device_t* device, unsigned short product_id, unsigned short usage_page_gamepad, unsigned short usage_page_control) {
    if (device == NULL) {
        return -1;
    }

    hid_close(device->control);
    device->control = NULL;
    hid_close(device->gamepad);
    device->gamepad = NULL;

    struct hid_device_info* infos = hid_enumerate(HORI_HID_VENDOR_ID, product_id);
    for (struct hid_device_info* info = infos; info != NULL; info = info->next) {
        if (info->vendor_id != HORI_HID_VENDOR_ID) {
            continue;
        }
        struct hori_internal_string_list location;
        memset(&location, 0, sizeof(location));
        location.value = hori_device_win32_get_hid_location_path(info->path, NULL);
        if (location.value == NULL) {
            continue;
        }
        location.byte_size = hori_internal_string_list_size(location.value, &location.count);
        if (location.byte_size != device->platform_data->location.byte_size ||
            location.count != device->platform_data->location.count ||
            memcmp(location.value, device->platform_data->location.value, location.byte_size) != 0) {
            free(location.value);
            continue;
        }
        free(location.value);
        if (info->usage_page == usage_page_gamepad && device->gamepad == NULL) {
            device->gamepad = hid_open_path(info->path);
        }
        else if (info->usage_page == usage_page_control && device->control == NULL) {
            device->control = hid_open_path(info->path);
        }
        if (device->control && device->gamepad) {
            break;
        }
    }
    hid_free_enumeration(infos);
    if (device->control || device->gamepad) {
        return 1;
    }
    return -1;
}

hori_device_config_t* hori_get_device_config(hori_device_t* device) {
    if (device == NULL || device->gamepad == NULL) {
        return NULL;
    }
    return device->config;
}

