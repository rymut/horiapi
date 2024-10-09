#include "hori_gamepad.h"

#include <stdlib.h>

#include <hidapi/hidapi.h>

#include "horiapi.h"
#include "hori_device.h"

hori_gamepad_t* HORI_API_CALL hori_make_gamepad() {
    return (hori_gamepad_t*)calloc(1, sizeof(hori_gamepad_t));
}

void HORI_API_CALL hori_free_gamepad(hori_gamepad_t* gamepad) {
    free(gamepad);
}

int HORI_API_CALL hori_read_gamepad(hori_device_t* device, hori_gamepad_t* gamepad) {
    if (device == NULL) {
        return -1;
    }
    if (device->gamepad == NULL) {
        return -1;
    }
    uint8_t report[sizeof(((hori_gamepad_t*)0)->report)];
    memset(report, 0, sizeof(report));
    int report_size = hid_read(device->gamepad, report, sizeof(report));
    if (gamepad) {
        int state = hori_get_state(device);
        if (gamepad->device_product != device->config->product ||
            gamepad->device_config_mode != device->config->device_config_mode ||
            gamepad->device_state != state) {
            memset(gamepad, 0, sizeof(hori_gamepad_t));
        }
        gamepad->device_product = device->config->product;
        gamepad->device_config_mode = device->config->device_config_mode;
        gamepad->device_state = state;
        memcpy(gamepad->report.raw, report, report_size);
    }
    return report_size;
}

int HORI_API_CALL hori_read_gamepad_timeout(hori_device_t* device, hori_gamepad_t* gamepad, int miliseconds) {
    if (device == NULL) {
        return -1;
    }
    if (device->gamepad == NULL) {
        return -1;
    }
    uint8_t report[sizeof(((hori_gamepad_t*)0)->report)];
    memset(report, 0, sizeof(report));
    int report_size = hid_read_timeout(device->gamepad, report, sizeof(report), miliseconds);
    if (gamepad) {
        int state = hori_get_state(device);
        if (gamepad->device_product != device->config->product ||
            gamepad->device_config_mode != device->config->device_config_mode ||
            gamepad->device_state != state) {
            memset(gamepad, 0, sizeof(hori_gamepad_t));
        }
        gamepad->device_product = device->config->product;
        gamepad->device_config_mode = device->config->device_config_mode;
        gamepad->device_state = state;
        memcpy(gamepad->report.raw, report, report_size);
    }
    int s = sizeof(struct hori_ps4_touch_finger_data);
    struct hori_ps4_touch_finger_data a;
    return report_size;
}

