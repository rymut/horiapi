#include <horiapi/horiapi.h>

#include <time.h>

#include "hori_time.h"
#include "hori_device.h"
#include "hori_context.h"

int HORI_API_CALL hori_get_state(hori_device_t* device) {
    if (device == NULL) {
        return HORI_STATE_NONE;
    }
    if (device->gamepad == NULL) {
        return HORI_STATE_NONE;
    }
    struct hid_device_info* dev_info = hid_get_device_info(device->gamepad);
    hori_device_config_t* device_config = hori_internal_find_device_config(device->context->devices, dev_info);
    if (device_config == NULL) {
        return HORI_STATE_NONE;
    }
    if (device_config->hid_normal_product_id == dev_info->product_id) {
        return HORI_STATE_NORMAL;
    }
    if (device_config->hid_config_product_id == dev_info->product_id && device->control != NULL) {
        return HORI_STATE_CONFIG;
    }
    return HORI_STATE_NONE;
}

int HORI_API_CALL hori_set_state(hori_device_t* device, int state) {
    if (device == NULL) {
        return HORI_STATE_NONE;
    }
    int current = hori_get_state(device);
    if (current == state) {
        return state;
    }
    hori_device_config_t* device_config = hori_get_device_config(device);
    unsigned short product_id = 0;
    unsigned short usage_page_gamepad = 0;
    unsigned short usage_page_control = 0;
    if (device_config == NULL) {
        return HORI_STATE_NONE;
    }
    if (current == HORI_STATE_CONFIG) {
        hori_internal_leave_config(device);
        product_id = device_config->hid_normal_product_id;
        usage_page_gamepad = device_config->hid_normal_usage_page_gamepad;
        usage_page_control = device_config->hid_normal_usage_page_control;
    }
    if (current == HORI_STATE_NORMAL) {
        if (-1 == hori_internal_send_enter_config(device)) {
            return current;
        }
        product_id = device_config->hid_config_product_id;
        usage_page_gamepad = device_config->hid_config_usage_page_gamepad;
        usage_page_control = device_config->hid_config_usage_page_profile;
    }
    hid_close(device->gamepad);
    device->gamepad = NULL;
    hid_close(device->control);
    device->control = NULL;


    for (hori_clock_t prev = hori_clock_now(), now = prev;
        hori_clock_diff(prev, now) * 1000 < device->context->rediscover_miliseconds_timeout;
        hori_sleep_ms(device->context->rediscover_miliseconds_delay), now = hori_clock_now()) {
        if (-1 != hori_internal_open(device, product_id, usage_page_gamepad, usage_page_control)) {
            if (-1 == hori_send_heartbeat(device)) {
                hid_close(device->gamepad);
                device->gamepad = NULL;
                hid_close(device->control);
                device->control = NULL;
                return HORI_STATE_NONE;
            }
            return state;
        }
    }
    return HORI_STATE_NONE;
}
