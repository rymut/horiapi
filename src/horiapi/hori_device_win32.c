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

BOOL hori_device_win32_get_device_interface_property(LPCWSTR path, const DEVPROPKEY* prop, DEVPROPTYPE buffer_type, PVOID buffer, ULONG buffer_size) {
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

LPWSTR hori_device_win32_get_device_interface_property_string_list(LPCWSTR path, const DEVPROPKEY* prop, struct hori_wstring_list* list) {
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
    if (hori_device_win32_get_device_interface_property(path, prop, buffer_type, buffer, buffer_size) == FALSE)
    {
        free(buffer);
        buffer = NULL;
    }
    if (list) {
        if (buffer) {
            list->value = buffer;
            list->length = buffer_size / sizeof(wchar_t);
            list->count = hori_wstring_list_count(buffer, buffer_size / sizeof(wchar_t));
        }
        else {
            hori_init_wstring_list(list);
        }
    }
    return buffer;
}

// @TODO remove
ULONG get_device_instance_property_size(DEVINST deviceInstance, const DEVPROPKEY* propertyKey) {
    if (propertyKey == NULL) {
        return 0;
    }
    DEVPROPTYPE type = DEVPROP_TYPE_NULL;
    ULONG size = 0;
    CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, NULL, &size, 0);
    return size;
}

// @TODO remove
DEVPROPTYPE get_device_interface_property_type(LPCWSTR path, const DEVPROPKEY* prop) {
    if (prop == NULL) {
        return 0;
    }
    ULONG size = 0;
    DEVPROPTYPE type = DEVPROP_TYPE_EMPTY;
    if (CR_BUFFER_SMALL != CM_Get_Device_Interface_PropertyW(path, prop, &type, NULL, &size, 0)) {
        return 0;
    }
    return type;
}

// @todo remove
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


LPWSTR hori_device_win32_get_device_interface_property_string(LPCWSTR path, const PDEVPROPKEY prop) {
    const DEVPROPTYPE buffer_type = DEVPROP_TYPE_STRING;
    if (get_device_interface_property_type(path, prop) != buffer_type) {
        return NULL;
    }
    const ULONG buffer_size = get_device_interface_property_size(path, prop);
    if (buffer_size == 0) {
        return NULL;
    }
    LPWSTR buffer = (LPWSTR)calloc(buffer_size, sizeof(char));
    if (buffer == NULL) {
        return NULL;
    }
    if (hori_device_win32_get_device_interface_property(path, prop, buffer_type, buffer, buffer_size) == FALSE)
    {
        free(buffer);
        buffer = NULL;
    }
    return buffer;
}

LPWSTR hori_win32_device_instance_get_string_list_property(DEVINST deviceInstance, const PDEVPROPKEY propertyKey, struct hori_wstring_list* list) {
    DEVPROPTYPE type = DEVPROP_TYPE_NULL;
    ULONG size = 0;
    CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, NULL, &size, 0);
    if (type != DEVPROP_TYPE_STRING_LIST || size < 2)
        return NULL;

    PBYTE propertyValue = (PBYTE)calloc(size, sizeof(char));
    if (propertyValue == NULL) {
        return NULL;
    }
    if (CR_SUCCESS != CM_Get_DevNode_PropertyW(deviceInstance, propertyKey, &type, propertyValue, &size, 0)) {
        free(propertyValue);
        propertyValue = NULL;
    }
    if (list) {
        list->value = (wchar_t*)propertyValue;
        if (propertyValue) {
            list->length = size / sizeof(wchar_t);
            list->count = hori_wstring_list_count(list->value, size / sizeof(wchar_t));
        }
    }
    return propertyValue;
}

PBYTE hori_win32_device_instance_get_property(DEVINST deviceInstance, const DEVPROPKEY* propertyKey) {
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

struct hori_wstring_list hori_device_win32_get_device_interfaces_wstring_list(const wchar_t* device_id, LPCGUID interface_class_guid)
{
    struct hori_wstring_list wstring_list;
    hori_init_wstring_list(&wstring_list);

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
        return wstring_list;
    }

    wchar_t* list = (wchar_t*)calloc(string_list_element_count, sizeof(wchar_t));
    if (list == NULL) {
        return wstring_list;
    }
    SIZE_T;
    if (CR_SUCCESS != CM_Get_Device_Interface_ListW(interface_class_guid, device_id, list, string_list_element_count, CM_GET_DEVICE_INTERFACE_LIST_PRESENT)) {
        free(list);
        return wstring_list;
    }

    wstring_list.value = list;
    wstring_list.length = string_list_element_count;
    wstring_list.count = hori_wstring_list_count(list, string_list_element_count);
    return wstring_list;
}


wchar_t* hori_device_win32_get_physical_device_intance_id(const char* path) {
    wchar_t* device_instance_id = NULL;
    hori_device_win32_get_physical_device(path, &device_instance_id, NULL);
    return device_instance_id;
}

struct hori_wstring_list hori_device_win32_get_physical_device_location_paths(const char* path) {
    struct hori_wstring_list list = hori_empty_wstring_list();
    hori_device_win32_get_physical_device(path, NULL, &list);
    if (list.count == 0)
        hori_clear_wstring_list(&list);
    return list;
}

INT hori_device_win32_get_device_node(const char* path, PDEVINST devNode) {
    if (path == NULL)
        return -1;
    wchar_t* wpath = hori_internal_UTF8toUTF16(path);
    if (wpath == NULL)
        return -1;
    wchar_t* deviceId = hori_device_win32_get_device_interface_property_string(wpath, &DEVPKEY_Device_InstanceId);
    free(wpath);
    wpath = NULL;

    if (deviceId == NULL)
        return -1;

    CONFIGRET cr = CM_Locate_DevNodeW(devNode, (DEVINSTID_W)deviceId, CM_LOCATE_DEVNODE_NORMAL);
    free(deviceId);
    deviceId = NULL;
    if (cr != CR_SUCCESS)
        return -1;
    return 0;
}

int hori_device_win32_is_physical_device(DEVINST devNode) {
    const wchar_t prefix[] = L"\\Device\\USBPDO-";
    wchar_t* pdoName = (wchar_t*)hori_win32_device_instance_get_property(devNode, &DEVPKEY_Device_PDOName);
    if (pdoName == NULL) {
        return 0;
    }
    if (wcsncmp(prefix, pdoName, sizeof(prefix) / sizeof(wchar_t)) == 0) {
        free(pdoName);
        pdoName = NULL;
        return 0;
    }
    free(pdoName);
    pdoName = NULL;
    return 1;
}

int hori_device_win32_is_controller(DEVINST devNode) {
    const GUID XnaCompositeClass = { 0xd61ca365, 0x5af4, 0x4486, { 0x99, 0x8b, 0x9d, 0xb4, 0x73, 0x4c, 0x6c, 0xa3} };
    GUID* classGuid = (GUID*)hori_win32_device_instance_get_property(devNode, &DEVPKEY_Device_ClassGuid);
    if (classGuid == NULL)
        return 0;
    const int result = IsEqualGUID(classGuid, &GUID_DEVCLASS_USB) || IsEqualGUID(classGuid, &XnaCompositeClass);
    free(classGuid);
    return result;
}

/** @brief Get information about physical device

    @param path[in] The device or device endpoint to get physical device info
    @param device_instance_id[out] Pointer to empty pointer where device instance id will be stored
    @param location_paths[out] Pointer to empty string list where data locations will be stored
 */
int hori_device_win32_get_physical_device(const char* path, wchar_t** device_instance_id, struct hori_wstring_list* location_paths) {

    DEVINST devNode = 0;
    if (hori_device_win32_get_device_node(path, &devNode) != 0) {
        return -1;
    }

    for (DEVINST devParent = devNode, devChild = 0; devParent != devChild; devChild = devParent, CM_Get_Parent(&devParent, devChild, 0))
    {
        if (!hori_device_win32_is_physical_device(devParent)) {
            continue;
        }

        if (!hori_device_win32_is_controller(devParent)) {
            continue;
        }
        struct hori_wstring_list list = hori_empty_wstring_list();
        if (hori_win32_device_instance_get_string_list_property(devParent, &DEVPKEY_Device_LocationPaths, &list)) {
            if (location_paths) {
                *location_paths = list;
            }
            else {
                hori_clear_wstring_list(&list);
            }
            if (device_instance_id) {
                *device_instance_id = (wchar_t*)hori_win32_device_instance_get_property(devParent, &DEVPKEY_Device_InstanceId);
            }
            return 0;
        }
    }
    return -1;
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
    wchar_t* deviceId = hori_device_win32_get_device_interface_property_string(path, &DEVPKEY_Device_InstanceId);
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
        wchar_t* pdoName = (wchar_t*)hori_win32_device_instance_get_property(devParent, &DEVPKEY_Device_PDOName);
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

        GUID* classGuid = (GUID*)hori_win32_device_instance_get_property(devParent, &DEVPKEY_Device_ClassGuid);
        if (classGuid == NULL) {
            continue;
        }
        if (!(IsEqualGUID(classGuid, &GUID_DEVCLASS_USB) || IsEqualGUID(classGuid, &XnaComposite))) {
            free(classGuid);
            continue;
        }
        free(classGuid);

        wchar_t* locationPaths = (wchar_t*)hori_win32_device_instance_get_property(devParent, &DEVPKEY_Device_LocationPaths);
        if (locationPaths) {
            if (device_instance != NULL) {
                free(*device_instance);
                *device_instance = (wchar_t*)hori_win32_device_instance_get_property(devParent, &DEVPKEY_Device_InstanceId);
            }
            return locationPaths;
        }
    }
    return NULL;
}

hori_device_platform_data_t* hori_make_platform_data(const hori_device_config_t* device_config, const char* path) {
    struct hori_wstring_list location = hori_device_win32_get_physical_device_location_paths(path);
    if (location.count == 0) {
        hori_clear_wstring_list(&location);
        return NULL;
    }
    hori_device_platform_data_t* platform_data = (hori_device_platform_data_t*)calloc(1, sizeof(hori_device_platform_data_t));
    if (platform_data == NULL) {
        hori_clear_wstring_list(&location);
        return NULL;
    }
    platform_data->location = location;
    return platform_data;
}

int hori_platform_data_is_identical(const hori_device_platform_data_t* a, const hori_device_platform_data_t* b) {
    if (a == NULL || b == NULL) {
        return a == b;
    }
    return a->location.length == b->location.length &&
        a->location.count == b->location.count &&
        a->location.value != NULL && b->location.value != NULL &&
        wmemcmp(a->location.value, b->location.value, a->location.length) == 0;
}

void hori_free_platform_data(hori_device_platform_data_t* data) {
    if (data) {
        free(data->location.value);
    }
    free(data);
}

