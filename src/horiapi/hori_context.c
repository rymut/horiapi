#include "hori_context.h"

#include <hidapi/hidapi.h>

static hori_device_config_t hori_known_devices[] = {
    {
        // pc mode
        "Hori_SPF023",
        HORI_PRODUCT_SPF_023,
        HORI_MODE_XINPUT,
        // device
        0x0164,
        // config
        0x021A,
        // usage
        0x0001,
        0x0,
        0x0001,
        0xFFF0,
        hori_known_devices + 1,
    },
    {
        // ps4 mode
        "Hori_SPF023",
        HORI_PRODUCT_SPF_023,
        HORI_MODE_PS4,
        // device
        0x0162,
        // config
        0x021B,
        // usage
        0x0001,
        0xFFF0,
        0x0001,
        0xFFF0,
        hori_known_devices + 2,
    },
    {	// ps5 mode
        "Hori_SPF023",
        HORI_PRODUCT_SPF_023,
        HORI_MODE_PS5,
        // device
        0x0163,
        // config
        0x021C,
        // usage
        0x0001,
        0xFFF0,
        0x0001,
        0xFFF0,
        NULL
    },
};

static const hori_context_t hori_default_context = {
    hori_known_devices,
    2000,
    20,
};

hori_context_t* hori_context() {
    return NULL;
}


const hori_context_t* hori_internal_context() {
    return &hori_default_context;
}

hori_device_config_t* hori_internal_find_device_config(hori_device_config_t* list_front, struct hid_device_info* info) {
    if (info == NULL) {
        return NULL;
    }
    for (hori_device_config_t* list_item = list_front; list_item != NULL; list_item = list_item->next) {
        if (info->vendor_id != HORI_HID_VENDOR_ID) {
            continue;
        }
        if (info->product_id == list_item->hid_normal_product_id && info->usage_page == list_item->hid_normal_usage_page_gamepad) {
            return list_item;
        }
        if (info->product_id == list_item->hid_config_product_id && info->usage_page == list_item->hid_config_usage_page_gamepad) {
            return list_item;
        }
    }
    return NULL;
}

