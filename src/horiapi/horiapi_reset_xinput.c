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


wchar_t* hori_device_win32_get_physical_device_intance_id(const char* path);
static uint8_t hori_internal_get_firmware_command[] = { 15, 0, 0, 60, 9 };


// FORM FEAD 'A' 'P' 'P' 
//static uint8_t hori_internal_enter_config_command_steam[] = { 0, 5, 12, 65, 80, 80 };
static uint8_t hori_internal_enter_config_command_ps5[] = { 2, 12, 65, 80, 80 };
static uint8_t hori_internal_enter_config_command_ps4[] = { 5, 12, 65, 80, 80 };


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


static BOOL get_device_interface_property_guid(LPCWSTR path, const DEVPROPKEY* prop, GUID* value) {
    GUID result;
    GUID* buffer = &result;
    const ULONG buffer_size = sizeof(GUID);
    const DEVPROPTYPE buffer_type = DEVPROP_TYPE_GUID;
    if (value) {
        buffer = value;
    }
    return hori_device_win32_get_device_interface_property(path, prop, buffer_type, buffer, buffer_size);
}


#include <inttypes.h>



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

hori_device_config_t* hori_get_device_config(hori_device_t* device) {
    if (device == NULL || device->gamepad == NULL) {
        return NULL;
    }
    return device->config;
}

