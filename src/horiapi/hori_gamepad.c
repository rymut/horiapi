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
            gamepad->device_controller != device->config->device_config_mode ||
            gamepad->device_state != state) {
            memset(gamepad, 0, sizeof(hori_gamepad_t));
        }
        gamepad->device_product = device->config->product;
        gamepad->device_controller = device->config->device_config_mode;
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
            gamepad->device_controller != device->config->device_config_mode ||
            gamepad->device_state != state) {
            memset(gamepad, 0, sizeof(hori_gamepad_t));
        }
        gamepad->device_product = device->config->product;
        gamepad->device_controller = device->config->device_config_mode;
        gamepad->device_state = state;
        memcpy(gamepad->report.raw, report, report_size);
    }
    int s = sizeof(struct hori_ps4_touch_finger_data);
    struct hori_ps4_touch_finger_data a;
    return report_size;
}

int hori_get_ps5_button(struct hori_ps5_gamepad_report* report, int button) {
    return -1;
}

enum hori_dpad {
    HORI_DPAD_UP = 0b0001,
    HORI_DPAD_LEFT = 0b0010,
    HORI_DPAD_DOWN = 0b0100,
    HORI_DPAD_RIGHT = 0b1000
};


const static int hori_playstation_hat[8] = {
    HORI_DPAD_UP,
    HORI_DPAD_UP | HORI_DPAD_RIGHT,
    HORI_DPAD_RIGHT,
    HORI_DPAD_RIGHT | HORI_DPAD_DOWN,
    HORI_DPAD_DOWN,
    HORI_DPAD_DOWN | HORI_DPAD_LEFT,
    HORI_DPAD_LEFT,
    HORI_DPAD_LEFT | HORI_DPAD_UP,
};

int hori_get_ps4_button(struct hori_ps4_gamepad_report* report, int button) {
    if (report == NULL) {
        return -1;
    }
    switch (button) {
    case HORI_PLAYSTATION_BUTTON_UP:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_UP) == HORI_DPAD_UP;
    case HORI_PLAYSTATION_BUTTON_DOWN:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_DOWN) == HORI_DPAD_DOWN;
    case HORI_PLAYSTATION_BUTTON_LEFT:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_LEFT) == HORI_DPAD_LEFT;
    case HORI_PLAYSTATION_BUTTON_RIGHT:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_RIGHT) == HORI_DPAD_RIGHT;
    case HORI_PLAYSTATION_BUTTON_CREATE:
    case HORI_PLAYSTATION_BUTTON_SHARE:
        return report->buttons.options;
    case HORI_PLAYSTATION_BUTTON_TOUCH_PAD:
        return report->buttons.tpad;
    case HORI_PLAYSTATION_BUTTON_OPTIONS:
        return report->buttons.options;
    case HORI_PLAYSTATION_BUTTON_CROSS:
        return report->buttons.cross;
    case HORI_PLAYSTATION_BUTTON_CIRCLE:
        return report->buttons.circle;
    case HORI_PLAYSTATION_BUTTON_TRIANGLE:
        return report->buttons.triangle;
    case HORI_PLAYSTATION_BUTTON_SQUARE:
        return report->buttons.square;
    case HORI_PLAYSTATION_BUTTON_RSTICK_UP:
        return report->right_stick[0] > 128;
    case HORI_PLAYSTATION_BUTTON_RSTICK_DOWN:
        return report->right_stick[0] < 128;
    case HORI_PLAYSTATION_BUTTON_RSTICK_LEFT:
        return report->right_stick[1] < 128;
    case HORI_PLAYSTATION_BUTTON_RSTICK_RIGHT:
        return report->right_stick[1] > 128;
    case HORI_PLAYSTATION_BUTTON_R3:
        return report->buttons.r3;
    case HORI_PLAYSTATION_BUTTON_PLAYSTATION_PS:
        return report->buttons.ps;
    case HORI_PLAYSTATION_BUTTON_PLAYSTATION_MUTE:
        // ??
        return -1;
    case HORI_PLAYSTATION_BUTTON_LSTICK_UP:
        return report->left_stick[0] > 128;
    case HORI_PLAYSTATION_BUTTON_LSTICK_DOWN:
        return report->left_stick[0] < 128;
    case HORI_PLAYSTATION_BUTTON_LSTICK_LEFT:
        return report->left_stick[1] < 128;
    case HORI_PLAYSTATION_BUTTON_LSTICK_RIGHT:
        return report->left_stick[1] > 128;
    case HORI_PLAYSTATION_BUTTON_L3:
        return report->buttons.l3;
    case HORI_PLAYSTATION_BUTTON_R1:
        return report->buttons.r1;
    case HORI_PLAYSTATION_BUTTON_R2:
        return report->buttons.r2;
    case HORI_PLAYSTATION_BUTTON_L1:
        return report->buttons.r1;
    case HORI_PLAYSTATION_BUTTON_L2:
        return report->buttons.l2;
    }
    return -1;
}

int hori_get_xinput_button(struct hori_xinput_gamepad_report* report, int button) {
    return -1;
}

int hori_get_config_button(struct hori_config_gamepad_report* report, int button) {
    return -1;
}

int HORI_API_CALL hori_get_button(hori_gamepad_t* gamepad, int button) {
    if (gamepad == NULL) {
        return -1;
    }
    const int controller = button & gamepad->device_controller;
    if (!controller) {
        // wrong data
        return -1;
    }
    switch (controller) {
    case HORI_CONTROLLER_PLAYSTATION4:
        return hori_get_ps4_button(&gamepad->report.ps4, button);
    case HORI_CONTROLLER_PLAYSTATION5:
        return hori_get_ps5_button(&gamepad->report.ps5, button);
    case HORI_CONTROLLER_XINPUT:
        return hori_get_xinput_button(&gamepad->report.xinput, button);
    case HORI_CONTROLLER_CONFIG:
        return hori_get_config_button(&gamepad->report.config, button);
        //   case HORI_CONTROLLER_HID:
       //        return hori_get_hid_button(&gamepad->report.hid, button);
    }
    return -1;
}

int hori_get_ps4_buttons(struct hori_ps4_gamepad_report* report) {
    if (report == NULL) {
        return -1;
    }
    int buttons = 0;
    for (int button = HORI_PLAYSTATION_BUTTON_UP; button <= HORI_PLAYSTATION_BUTTON_L2; ++button) {
        int bit_mask = HORI_BUTTON_BIT_MASK(button);
        if (bit_mask <= 0) {
            break;
        }
        int state = hori_get_ps4_button(report, button);
        if (state == 1) {
            buttons = buttons | bit_mask;
        }
    }
    return buttons;
}
int HORI_API_CALL hori_get_buttons(hori_gamepad_t* gamepad) {
    if (gamepad == NULL) {
        return -1;
    }
    switch (gamepad->device_controller) {
    case HORI_CONTROLLER_PLAYSTATION4:
        return hori_get_ps4_buttons(&gamepad->report.ps4);
    }
    return -1;
}
