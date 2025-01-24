#include "hori_context.h"

#include <hidapi/hidapi.h>

// 0xFF - virtual or not connected
union hori_device_index_input common[] = {
    { 0, HORI_BUTTON_UP },
    { 1, HORI_BUTTON_DOWN },
    { 2, HORI_BUTTON_LEFT },
    { 3, HORI_BUTTON_RIGHT },

    { 4, HORI_BUTTON_L1, },
    { 5, HORI_BUTTON_R1, },
    { 6, HORI_BUTTON_L3, },
    { 7, HORI_BUTTON_R3, },

    { 8, HORI_BUTTON_FL1, },
    { 9, HORI_BUTTON_FL2 },
    { 10, HORI_BUTTON_FR1 },
    { 11, HORI_BUTTON_FR2 },

    { 12, HORI_BUTTON_L2 },
    { 13, HORI_BUTTON_R2 },

    { 14, HORI_BUTTON_CROSS },
    { 15, HORI_BUTTON_CIRCLE },
    { 16, HORI_BUTTON_SQUARE },
    { 17, HORI_BUTTON_TRIANGLE },

    { 18, HORI_BUTTON_LPEDAL },
    { 19, HORI_BUTTON_RPEDAL },

    { 20, HORI_BUTTON_PLAYSTATION },
    { 21, HORI_BUTTON_SELECT },
    { 22, HORI_BUTTON_START },
    { 23, HORI_BUTTON_CAMERA },

    { 0xFF, HORI_BUTTON_LSTICK_UP, },
    { 0xFF, HORI_BUTTON_LSTICK_DOWN},
    { 0xFF, HORI_BUTTON_LSTICK_LEFT},
    { 0xFF, HORI_BUTTON_LSTICK_RIGHT,},

    { 0xFF, HORI_BUTTON_RSTICK_UP},
    { 0xFF, HORI_BUTTON_RSTICK_DOWN},
    { 0xFF, HORI_BUTTON_RSTICK_LEFT},
    { 0xFF, HORI_BUTTON_RSTICK_RIGHT},

    { 0xFF, HORI_BUTTON_LAGILE},
    { 0xFF, HORI_BUTTON_LTARGET},

    // COSTOM BUT NOT MAPPED
    { 0xFF, HORI_BUTTON_SHARE},
    { 0xFF, HORI_BUTTON_OPTIONS },
    // DISABLE MAPPING
    { 0xFF, HORI_BUTTON_DISABLED }
};


static hori_device_config_t hori_known_devices[] = {
    {
        // pc mode
        "Hori_SPF023",
        HORI_PRODUCT_SPF_023,
        HORI_CONTROLLER_XINPUT,
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
        HORI_CONTROLLER_PLAYSTATION4,
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
        HORI_CONTROLLER_PLAYSTATION5,
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
    .devices = hori_known_devices,
    .rediscover_miliseconds_timeout = 2000,
    .rediscover_miliseconds_delay = 10,
    .read_timeout_ms = 50,
    .retry_read_timeout_ms = 5,
    .retry_attempts = 10,
};

// @todo different implementation probably copy from hori_internal_context
hori_context_t* hori_context() {
    return hori_internal_context();
}


const hori_context_t* hori_internal_context() {
    return &hori_default_context;
}

hori_device_config_t* hori_internal_device_config_find(hori_device_config_t* list_front, struct hid_device_info* info) {
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

