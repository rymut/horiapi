#include <horiapi/horiapi.h>

#include <hidapi/hidapi.h>
#include <stdlib.h>
#include <Windows.h>
#include <hidsdi.h>
#pragma comment(lib, "hid.lib")
// declare settings for context

#include "hori_device.h"
#include "hori_context.h"

void* copy(void* value, int size) {
    if (value == NULL) {
        return NULL;
    }
    char* result = (char*)malloc((size + 1) * sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, value, size * sizeof(char));
    result[size] = 0;
    return result;
}

char* strcopy(char* value) {
    if (value == NULL) {
        return NULL;
    };
    return (char*)copy(value, strlen(value));
}

wchar_t* wstrcopy(wchar_t* value) {
    if (value == NULL)
    {
        return NULL;
    }
    int len = wcslen(value);
    return (wchar_t*)copy(value, len);
}

static int match_hid_id(struct hid_device_info* hid_info, unsigned short vendor_id, unsigned short product_id) {
    if (hid_info == NULL) {
        return 0;
    }
    return hid_info->product_id == product_id && hid_info->vendor_id == vendor_id;
}

hori_enumeration_t* hori_enumerate(hori_product_t product, hori_context_t* context) {
    struct hid_device_info* list_front = hid_enumerate(HORI_HID_VENDOR_ID, 0);
    hori_context_t* ctx = context ? context : hori_internal_context();
    hori_enumeration_t* front = NULL;
    hori_enumeration_t** back = &front;

    for (struct hid_device_info* list_item = list_front; list_item != NULL; list_item = list_item->next) {
        hori_device_config_t* device_config = hori_internal_device_config_find(ctx->devices, list_item);
        if (device_config == NULL) {
            continue;
        }
        if (product != HORI_PRODUCT_ANY && product != device_config->product) {
            continue;
        }
        int state = HORI_STATE_NONE;
        if (device_config->hid_config_product_id == list_item->product_id) {
            state = HORI_STATE_CONFIG;
        }
        else if (device_config->hid_normal_product_id == list_item->product_id) {
            state = HORI_STATE_NORMAL;
        }
        hori_enumeration_t* item = (hori_enumeration_t*)calloc(1, sizeof(hori_enumeration_t));
        if (item == NULL) {
            continue;
        }
        item->path = strdup(list_item->path);
        item->manufacturer_string = _wcsdup(list_item->manufacturer_string);
        item->product_string = _wcsdup(list_item->product_string);
        item->device_config = device_config;
        *back = item;
        back = &item->next;
    }
    return front;
}

void hori_free_enumerate(hori_enumeration_t* devices) {
    if (!devices) {
        return;
    }
    hori_enumeration_t* device = devices;
    while (device != NULL) {
        hori_enumeration_t* item = device;
        device = device->next;
        free(item->path);
        free(item->manufacturer_string);
        free(item->product_string);
        free(item);
    }
}

hori_device_config_t* hori_device_config(struct hid_device_info* _device, hori_device_config_t* _supported, size_t count) {
    if (_device == NULL) {
        return NULL;
    }
    if (_supported == NULL || count == 0) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        if (HORI_HID_VENDOR_ID == _device->vendor_id && _supported[i].hid_normal_product_id == _device->product_id) {
            return _supported + i;
        }
        if (HORI_HID_VENDOR_ID == _device->vendor_id && _supported[i].hid_config_product_id == _device->product_id) {
            return _supported + i;
        }
    }
    return NULL;
}

static int match_hid_device(struct hid_device_info* hid_info, hori_device_info_t* hori_info) {
    if (hid_info == NULL || hori_info == NULL || hori_info->device_config == NULL) {
        return 0;
    }
    if (match_hid_id(hid_info, HORI_HID_VENDOR_ID, hori_info->device_config->hid_config_product_id)) {
        return 1;
    }
    return 0;
}

struct hid_device_info* next(struct hid_device_info* list, hori_device_info_t* info, hori_device_config_t* _devices, size_t count) {
    if (list == NULL) {
        return NULL;
    }
    if (_devices == NULL || count == 0) {
        return NULL;
    }
    for (struct hid_device_info* item = list; item != NULL; item = item->next) {
        hori_device_config_t* item_info = hori_device_config(item, _devices, count);
        if (item_info == NULL) {
            continue;
        }
    }
    return NULL;
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
    hori_device_config_t* device_config = hori_internal_device_config_find(ctx->devices, hid_dev_info);
    if (device_config == NULL) {
        hid_close(hid_dev);
        return NULL;
    }
    unsigned short product_id = hid_dev_info->product_id;
    hid_close(hid_dev);
    hori_device_platform_data_t* platform_data = hori_make_platform_data(device_config, path);
    if (platform_data == NULL) {
        return NULL;
    }
    hori_device_t* device = (hori_device_t*)calloc(1, sizeof(hori_device_t));
    if (device == NULL) {
        free(platform_data);
        return NULL;
    }
    device->hori_api_version = HORI_API_VERSION;
    device->platform_data = platform_data;
    device->context = ctx;
    device->config = device_config;

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

int hori_send_heartbeat(hori_device_t* device)
{
    static uint8_t hori_internal_heartbeat_command[] = { 15, 170, 85, 90, 165 };
    if (device == NULL) {
        return -1;
    }
    if (device->control == NULL) {
        return -1;
    }
    int write = hid_write(device->control, hori_internal_heartbeat_command, sizeof(hori_internal_heartbeat_command));
    return write;
}

