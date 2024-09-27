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
wchar_t* hori_internal_hid_location_path(const char* hid_path, wchar_t** device_instance);

static uint8_t hori_internal_get_firmware_command[] = { 15, 0, 0, 60, 9 };

static uint8_t hori_internal_heartbeat_command[] = { 15, 170, 85, 90, 165 };

int hori_read_firmware_version(hori_device_t* device, char* out, int version_size) {
    if (device == NULL || device->control == NULL) {
        return -1;
    }

    if (-1 == hid_write(device->control, hori_internal_get_firmware_command, sizeof(hori_internal_get_firmware_command))) {
        return -1;
    }
    if (-1 == hid_write(device->control, hori_internal_heartbeat_command, sizeof(hori_internal_heartbeat_command))) {
        return -1;
    }

    uint8_t buffer[65];
    memset(buffer, 0, sizeof(buffer));
    int read = hid_read_timeout(device->control, buffer, sizeof(buffer) - 1, 25);
    if (read == -1) {
        return -1;
    }
    if (buffer[4] != 10) // ack
    {
        return -1;
    }
    int size = 0;
    char version[64];
    memset(version, 0, sizeof(version));
    char* offset = version;
    uint8_t* ptr = buffer + 5;
    for (int p = 0, len = strlen(ptr); len != 0; ptr = ptr + len + 1, p++, len = strlen(ptr)) {
        if (size == 0 && p == 0) {
            if (len == 1 && ptr[0] == '0') {
                continue;
            }
        }
        if (offset != version) {
            *offset = (p < 4) ? '.' : '-';
            offset += 1;
        }
        memcpy(offset, ptr, len);
        offset = offset + len;
        
    }

    size = offset != version ? offset - version + 1: 0;
    if (version_size == 0) {
        return size;
    }
    if (version_size < size || out == NULL) {
        return -1;
    }
    memcpy(out, version, size);
    return size;
}
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
    wchar_t* locationPath = hori_internal_hid_location_path(hid_get_device_info(device->gamepad)->path, &device_id);
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

static PBYTE hori_internal_device_instance_get_property(DEVINST deviceInstance, const DEVPROPKEY* propertyKey) {
    DEVPROPTYPE type = DEVPROP_TYPE_NULL;
    ULONG size = 0;
    CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, NULL, &size, 0);

    if (size == 0) {
        return NULL;
    }
    PBYTE propertyValue = (PBYTE)calloc(size, sizeof(char));
    if (propertyValue == NULL) {
        return NULL;
    }
    if (CR_SUCCESS != CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, propertyValue, &size, 0)) {
        free(propertyValue);
        propertyValue = NULL;
    }
    return propertyValue;
}

static wchar_t* hori_internal_UTF8toUTF16(const char* utf8) {
    wchar_t* utf16 = NULL;
    int result = 0, error = 0;
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, NULL, 0);
    if (len) {
        utf16 = (wchar_t*)calloc(len, sizeof(wchar_t));
        if (utf16 == NULL) {
            return NULL;
        }
        FORMAT_MESSAGE_FROM_SYSTEM;
        result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, utf16, len);
        error = GetLastError();
    }
    return utf16;
}

static char* hori_internal_UTF16toUTF8(const wchar_t* utf16)
{
    char* utf8 = NULL;
    int len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16, -1, NULL, 0, NULL, NULL);
    if (len) {
        utf8 = (char*)calloc(len, sizeof(char));
        if (utf8 == NULL) {
            return NULL;
        }
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16, -1, utf8, len, NULL, NULL);
    }
    return utf8;
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

static BOOL get_device_interface_property(LPCWSTR path, const DEVPROPKEY* prop, DEVPROPTYPE buffer_type, PVOID buffer, ULONG buffer_size) {
    if (prop == NULL) {
        return FALSE;
    }
    ULONG prop_size = get_device_interface_property_size(path, prop);
    if (buffer_size < prop_size) {
        return FALSE;
    }
    DEVPROPTYPE prop_type = get_device_interface_property_type(path, prop);
    if (buffer_type != prop_type) {
        return FALSE;
    }
    if (CR_SUCCESS != CM_Get_Device_Interface_PropertyW(
        path, prop, &prop_type, buffer, &prop_size, 0)) {
        return FALSE;
    }
    return TRUE;
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

static LPWSTR get_device_interface_property_string(LPCWSTR path, const DEVPROPKEY* prop) {
    const DEVPROPTYPE buffer_type = DEVPROP_TYPE_STRING;
    if (get_device_interface_property_type(path, prop) != buffer_type) {
        return NULL;
    }
    const ULONG buffer_size = get_device_interface_property_size(path, prop);
    if (buffer_size == 0) {
        return NULL;
    }
    LPWSTR buffer = (LPWSTR)malloc(buffer_size);
    if (buffer == NULL) {
        return NULL;
    }
    if (get_device_interface_property(path, prop, buffer_type, buffer, buffer_size) == FALSE)
    {
        free(buffer);
        buffer = NULL;
    }
    return buffer;
}
static LPWSTR get_device_interface_property_string_list(LPCWSTR path, const DEVPROPKEY* prop) {
    const DEVPROPTYPE buffer_type = DEVPROP_TYPE_STRING_LIST;
    if (get_device_interface_property_type(path, prop) != buffer_type) {
        return NULL;
    }
    const ULONG buffer_size = get_device_interface_property_size(path, prop);
    if (buffer_size == 0) {
        return NULL;
    }
    LPWSTR buffer = (LPWSTR)malloc(buffer_size);
    if (buffer == NULL) {
        return NULL;
    }
    if (get_device_interface_property(path, prop, buffer_type, buffer, buffer_size) == FALSE)
    {
        free(buffer);
        buffer = NULL;
    }
    return buffer;
}

static void test() {

    //myGUIDFromString(L"{d61ca365-5af4-4486-998b-9db4734c6ca3}", &GUID_DEVCLASS_TOASTER);
    //HidD_GetHidGuid(&GUID_DEVINTERFACE_TOASTER)
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

/**
 * @brief Return location string list for windows as wchar
 * @return pointer (needs to be free by user)
 */
wchar_t* hori_internal_hid_location_path(const char* hid_path, wchar_t** device_instance) {
    if (hid_path == NULL) {
        return NULL;
    }
    wchar_t* path = hori_internal_UTF8toUTF16(hid_path);
    if (path == NULL) {
        return NULL;
    }
    wchar_t* deviceId = get_device_interface_property_string(path, &DEVPKEY_Device_InstanceId);
    free(path); path = NULL;
    if (deviceId == NULL) {
        return NULL;
    }
    DEVINST devNode = 0;
    CONFIGRET result = CM_Locate_DevNodeW(&devNode, (DEVINSTID_W)deviceId, CM_LOCATE_DEVNODE_NORMAL);
    free(deviceId); deviceId = NULL;
    if (result != CR_SUCCESS) {
        return NULL;
    }
    const GUID XnaComposite = { 0xd61ca365, 0x5af4, 0x4486, { 0x99, 0x8b, 0x9d, 0xb4, 0x73, 0x4c, 0x6c, 0xa3} };
    for (DEVINST devParent = devNode, devChild = 0; devParent != devChild; devChild = devParent, CM_Get_Parent(&devParent, devChild, 0))
    {
        const wchar_t prefix[] = L"\\Device\\USBPDO-";
        sizeof(prefix) / sizeof(wchar_t);
        wchar_t* pdoName = (wchar_t*)hori_internal_device_instance_get_property(devParent, &DEVPKEY_Device_PDOName);
        if (pdoName == NULL) {
            continue;
        }
        if (wcsncmp(prefix, pdoName, sizeof(prefix) / sizeof(wchar_t)) == 0) {
            free(pdoName);
            pdoName = NULL;
            continue;
        }
        free(pdoName);
        pdoName = NULL;

        GUID* classGuid = (GUID*)hori_internal_device_instance_get_property(devParent, &DEVPKEY_Device_ClassGuid);
        if (classGuid == NULL) {
            continue;
        }
        if (!(IsEqualGUID(classGuid, &GUID_DEVCLASS_USB) || IsEqualGUID(classGuid, &XnaComposite))) {
            free(classGuid);
            continue;
        }
        free(classGuid);

        wchar_t* locationPaths = (wchar_t*)hori_internal_device_instance_get_property(devParent, &DEVPKEY_Device_LocationPaths);
        if (locationPaths) {
            if (device_instance != NULL) {
                free(*device_instance);
                *device_instance = (wchar_t*)hori_internal_device_instance_get_property(devParent, &DEVPKEY_Device_InstanceId);
            }
            return locationPaths;
        }
    }
    return NULL;
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
        location.value = hori_internal_hid_location_path(info->path, NULL);
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
    return hori_internal_find_device_config(device->context->devices, hid_get_device_info(device->gamepad));
}

hori_device_t* hori_open_path(char* path, hori_context_t* context) {
    hid_device* hid_dev = hid_open_path(path);
    if (hid_dev == NULL) {
        // error no device found
        return NULL;
    }
    struct hid_device_info* hid_dev_info = hid_get_device_info(hid_dev);
    if (hid_dev_info == NULL) {
        hid_close(hid_dev);
        return NULL;
    }
    hori_context_t* ctx = context ? context : hori_internal_context();
    hori_device_config_t* device_config = hori_internal_find_device_config(ctx->devices, hid_dev_info);
    if (device_config == NULL) {
        hid_close(hid_dev);
        return NULL;
    }
    unsigned short product_id = hid_dev_info->product_id;
    hid_close(hid_dev);
    hori_device_platform_data_t* platform_data = hori_internal_platform_data(device_config, path);
    if (platform_data == NULL) {
        return NULL;
    }
    hori_device_t* device = (hori_device_t*)calloc(1, sizeof(hori_device_t));
    if (device == NULL) {
        free(platform_data);
        return NULL;
    }
    device->platform_data = platform_data;
    device->context = ctx;

    int result = -1;
    if (device_config->hid_config_product_id == product_id) {
        result = hori_internal_open(device, device_config->hid_config_product_id, device_config->hid_config_usage_page_gamepad, device_config->hid_config_usage_page_profile);
    }
    else {
        result = hori_internal_open(device, device_config->hid_normal_product_id, device_config->hid_normal_usage_page_gamepad, device_config->hid_normal_usage_page_control);
    }
    if (result == -1) {
        hori_close(device);
    }
    return device;
}

hori_device_platform_data_t* hori_internal_platform_data(hori_device_config_t* device_config, char* path) {
    wchar_t* location = hori_internal_hid_location_path(path, NULL);
    if (location == NULL) {
        return NULL;
    }
    hori_device_platform_data_t* platform_data = (hori_device_platform_data_t*)calloc(1, sizeof(hori_device_platform_data_t));
    if (platform_data == NULL) {
        return NULL;
    }
    platform_data->location.value = location;
    platform_data->location.byte_size = hori_internal_string_list_size(location, &platform_data->location.count);
    return platform_data;
}

void hori_internal_free_platform_data(hori_device_platform_data_t* data) {
    if (data) {
        free(data->location.value);
    }
    free(data);
}
