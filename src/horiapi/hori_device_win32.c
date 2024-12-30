#include "hori_device_win32.h"

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
#include "hori_utf.h"

#pragma comment (lib, "Setupapi")
#pragma comment(lib, "Cfgmgr32")
#pragma comment(lib, "Hid")

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


/**
 * @brief Return location string list for windows as wchar
 * @return pointer (needs to be free by user)
 */
wchar_t* hori_device_win32_get_hid_location_path(const char* hid_path, wchar_t** device_instance) {
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

hori_device_platform_data_t* hori_make_platform_data(const hori_device_config_t* device_config, const char* path) {
    wchar_t* location = hori_device_win32_get_hid_location_path(path, NULL);
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

void hori_free_platform_data(hori_device_platform_data_t* data) {
    if (data) {
        free(data->location.value);
    }
    free(data);
}
