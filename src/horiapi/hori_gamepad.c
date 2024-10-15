#include "hori_gamepad.h"

#include <stdlib.h>

#include <hidapi/hidapi.h>

#include "horiapi.h"
#include "hori_device.h"

enum hori_function {
    HORI_FUNCTION_RAW,
    HORI_FUNCTION_DPAD,
    HORI_FUNCTION_AXIS,
};

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

/** @brief Match pressed buttons

    @param dpad The dpad structure, 0-2 bit clock wise direction change, 3 bit hat released
    @param directions The direction to match

    @returns
        This function returns 1 if dpad direction is pressed, 0 otherwise
  */
int hori_function_dpad(int dpad, int directions) {
    if (dpad < 0 || dpad >= 8) {
        return directions == 0;
    }
    return (hori_playstation_hat[dpad] & directions) == directions;
}

/** only 8 bits to set all flags
 */
enum hori_function_axis {
    HORI_ARG_AXIS_BINARIZE = 1, // return 0 or 1 on value for non zero value
    HORI_ARG_AXIS_REVERSED = 2, // value from high to low (255 relesead, 0 pressed) otherwise (0, 255)
    HORI_ARG_AXIS_SIGNED = 4, // axis returns signed value (half point is 0) so for 8 bits half value is 1<<(8-1) - this value is substracted from RAW
};

/** @brief Get Axis value
 */
long hori_function_axis(long value, int bit_count, int flags) {
    long raw = value;
    if (flags & HORI_ARG_AXIS_REVERSED) {
        int max = (1 << bit_count) - 1;
        raw = max - raw;
    }
    if (flags & HORI_ARG_AXIS_SIGNED) {
        int max = 1 << (bit_count - 1);
        raw = raw - max;
    }
    if (flags & HORI_ARG_AXIS_BINARIZE) {
        return raw > 0;
    }
    return raw;
}

struct hori_lookup {
    // address in report
    unsigned char index : 6;
    // endian used 0 - LE, 1 - BE, 2 (future use)
    unsigned char endian : 2;
    /** @brief Field length */
    unsigned char length : 5;
    // first bit numbe
    unsigned char bit : 3;
    // processing function
    unsigned char function;
    unsigned char arg;
};
#define HORI_MAKE_LOOKUP_NONE() { 0, 0, 0, 0, 0, 0 }
#define HORI_MAKE_LOOKUP_BUTTON(index, bit) { index, 0, 1, bit, HORI_FUNCTION_RAW, 0 }
#define HORI_MAKE_LOOKUP_DPAD(index, bit, arg) { index, 0, 4, bit, HORI_FUNCTION_DPAD, arg }
#define HORI_MAKE_LOOKUP_AXIS(index, bit, length, arg) { index, 0, length, bit, HORI_FUNCTION_AXIS, arg }

// bitmask
// address 0-63 (6 bits)
// bit 0-7 (3 bits)
// length: (5 bits)
//      - 0 - 1 bit
//      - 1 - 8 bit
//      - 2 - 16 bits
// signed:
//      - 0 unsigned
//      - 1 signed
// compute
//      - 0 raw
/*
*/
struct hori_lookup hori_config_generic_gamepad_lookup[] = {
    HORI_MAKE_LOOKUP_NONE(),
    // HORI_BUTTON_UP = 1
    HORI_MAKE_LOOKUP_DPAD(5, 3, HORI_DPAD_UP),
    // HORI_BUTTON_DOWN = 2,
    HORI_MAKE_LOOKUP_DPAD(5, 3, HORI_DPAD_DOWN),
    // HORI_BUTTON_LEFT = 3,
    HORI_MAKE_LOOKUP_DPAD(5, 3, HORI_DPAD_LEFT),
    // HORI_BUTTON_RIGHT = 4,
    HORI_MAKE_LOOKUP_DPAD(5, 3, HORI_DPAD_RIGHT),
    // HORI_BUTTON_L1 = 6,
    HORI_MAKE_LOOKUP_BUTTON(6, 0),
    // HORI_BUTTON_R1 = 6,
    HORI_MAKE_LOOKUP_BUTTON(6, 1),
    // HORI_BUTTON_L3 = 7,
    HORI_MAKE_LOOKUP_BUTTON(6, 6),
    // HORI_BUTTON_R3 = 8,
    HORI_MAKE_LOOKUP_BUTTON(6, 7),
    // HORI_BUTTON_FL1 = 9,
    HORI_MAKE_LOOKUP_BUTTON(7,2),
    // HORI_BUTTON_FL2 = 10,
    HORI_MAKE_LOOKUP_NONE(),
    // HORI_BUTTON_FR1 = 11,
    HORI_MAKE_LOOKUP_BUTTON(7, 3),
    // HORI_BUTTON_FR2 = 12,
    HORI_MAKE_LOOKUP_NONE(),
    // HORI_BUTTON_L2 = 13,
    HORI_MAKE_LOOKUP_BUTTON(6, 2),
    // HORI_BUTTON_R2 = 14,
    HORI_MAKE_LOOKUP_BUTTON(6, 3),
    // HORI_BUTTON_CROSS = 15,
    HORI_MAKE_LOOKUP_BUTTON(5, 5),
    // HORI_BUTTON_CIRCLE = 16,
    HORI_MAKE_LOOKUP_BUTTON(5, 6),
    // HORI_BUTTON_SQUARE = 17,
    HORI_MAKE_LOOKUP_BUTTON(5, 4),
    // HORI_BUTTON_TRIANGLE = 18,
    HORI_MAKE_LOOKUP_BUTTON(5, 7),
    // HORI_BUTTON_LPEDAL = 19,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, left_pedal), 0, 16, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_RPEDAL = 20,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, right_pedal), 0, 16, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_PLAYSTATION = 21,
    HORI_MAKE_LOOKUP_BUTTON(7, 0),
    // HORI_BUTTON_SELECT = 22,
    HORI_MAKE_LOOKUP_BUTTON(8, 1),
    // HORI_BUTTON_START = 23,
    HORI_MAKE_LOOKUP_BUTTON(8, 0),
    // HORI_BUTTON_CAMERA = 24,
    HORI_MAKE_LOOKUP_BUTTON(7, 1),
    // HORI_BUTTON_LSTICK_UP = 25,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, left_stick.x), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED),
    // HORI_BUTTON_LSTICK_DOWN = 26,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, left_stick.x), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_LSTICK_LEFT = 27,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, left_stick.y), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_LSTICK_RIGHT = 28,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, left_stick.y), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED),
    // HORI_BUTTON_RSTICK_UP = 29,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, right_stick.x), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED),
    // HORI_BUTTON_RSTICK_DOWN = 30,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, right_stick.x), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_RSTICK_LEFT = 31,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, right_stick.y), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED | HORI_ARG_AXIS_REVERSED),
    // HORI_BUTTON_RSTICK_RIGHT = 32,
    HORI_MAKE_LOOKUP_AXIS(offsetof(struct hori_config_spf023_gamepad_report, right_stick.y), 7, 8, HORI_ARG_AXIS_BINARIZE | HORI_ARG_AXIS_SIGNED),
    // HORI_BUTTON_LAGILE = 33,
    HORI_MAKE_LOOKUP_NONE(),
    // HORI_BUTTON_LTARGET = 34,
    HORI_MAKE_LOOKUP_NONE(),
};

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
        if (report_size > 0) {
            memcpy(gamepad->report.raw, report, report_size);
        }
    }
    return report_size;
}

int hori_get_ps5_button(struct hori_ps5_gamepad_report* report, int button) {
    return -1;
}

int hori_get_ps4_button(struct hori_ps4_gamepad_report* report, int button) {
    if (report == NULL) {
        return -1;
    }
    if (button <= 0 || (button & HORI_CONTROLLER_PLAYSTATION4) != HORI_CONTROLLER_PLAYSTATION4) {
        return -1;
    }

    switch (HORI_BUTTON_INDEX(button)) {
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_UP):
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_UP) == HORI_DPAD_UP;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_DOWN):
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_DOWN) == HORI_DPAD_DOWN;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_LEFT):
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_LEFT) == HORI_DPAD_LEFT;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_RIGHT):
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_RIGHT) == HORI_DPAD_RIGHT;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_CREATE):
        return report->buttons.share;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_TOUCH_PAD):
        return report->buttons.tpad;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_OPTIONS):
        return report->buttons.options;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_CROSS):
        return report->buttons.cross;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_CIRCLE):
        return report->buttons.circle;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_TRIANGLE):
        return report->buttons.triangle;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_SQUARE):
        return report->buttons.square;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_RSTICK_UP):
        return report->right_stick_x > 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_RSTICK_DOWN):
        return report->right_stick_x < 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_RSTICK_LEFT):
        return report->right_stick_y < 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_RSTICK_RIGHT):
        return report->right_stick_y > 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_R3):
        return report->buttons.r3;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_PLAYSTATION_PS):
        return report->buttons.ps;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_PLAYSTATION_MUTE):
        // ??
        return -1;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_LSTICK_UP):
        return report->left_stick_x > 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_LSTICK_DOWN):
        return report->left_stick_x < 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_LSTICK_LEFT):
        return report->left_stick_y < 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_LSTICK_RIGHT):
        return report->left_stick_y > 128;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_L3):
        return report->buttons.l3;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_R1):
        return report->buttons.r1;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_R2):
        return report->buttons.r2;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_L1):
        return report->buttons.l1;
    case HORI_BUTTON_INDEX(HORI_PLAYSTATION_BUTTON_L2):
        return report->buttons.l2;
    }
    return -1;
}

int hori_get_xinput_button(struct hori_xinput_gamepad_report* report, int button) {
    return -1;
}
/** @brief

    @param bit_index The value in <0, 7> range
    @param bit_size The value in <1, 31> range
 */
long hori_button_value(uint8_t* report, int report_size, int byte_index, int bit_index, int bit_size) {
    if (report == NULL || report_size <= 0) {
        return -1;
    }
    if (byte_index < 0 || bit_index < 0 || bit_index > 7 || bit_size < 1 || bit_size > 32) {
        return -1;
    }
    if (byte_index + (bit_size - bit_index + 8 / 2) / 8 >= report_size) {
        // out of report
        return -1;
    }
    int value = 0;
    int bit_offset = bit_index;
    // number of bits that are left
    int bit_remaining = bit_size;
    int bit_lshift = 7 - bit_index;
    // number of bits in left value
    int bit_left = bit_index + 1;
    int bit_used = bit_remaining > bit_left ? bit_left : bit_remaining;
    int bit_rshift = bit_left < bit_used ? bit_left : 8 - bit_used;

    // first byte
    int item = report[byte_index];
    byte_index++;
    item = item << bit_lshift;
    item = item >> bit_rshift;
    value = (value << 8) | item;
    bit_remaining -= bit_used;
    // next bytes are always full
    while (bit_remaining >= 8) {
        item = report[byte_index];
        byte_index++;
        value = (value << 8) | item;
        bit_remaining -= 8;
    }
    // last bit need to be computed
    bit_rshift = 0;
    if (bit_remaining > 0) {
        item = report[byte_index];
        bit_rshift = bit_lshift = (8 - bit_remaining);
        item = item >> bit_rshift;
        item = item << bit_lshift;
        value = (value << 8) | item;
    }
    value = value >> bit_rshift;
    return value;
}
int hori_get_config_button_lookup(uint8_t* report, int report_size, struct hori_lookup* lookups, int lookups_size, int button) {
    if (report == NULL || report_size < 64) {
        return -1;
    }
    int index = HORI_BUTTON_BIT_INDEX(button);
    if (index >= lookups_size) {
        return -1;
    }
    const struct hori_lookup lookup = lookups[index];
    if (lookup.index == 0) {
        return -1;
    }

    long value = hori_button_value(report, report_size, lookup.index, lookup.bit, lookup.length);
    if (lookup.function == HORI_FUNCTION_RAW) {
        return value;
    }
    if (lookup.function == HORI_FUNCTION_DPAD) {
        return hori_function_dpad(value, lookup.arg);
    }
    if (lookup.function == HORI_FUNCTION_AXIS) {
        return hori_function_axis(value, lookup.length, lookup.arg);
    }
    return -1;
}
int hori_get_config_button(union hori_gamepad_report* gamepad_report, int button) {
    if (gamepad_report == NULL) {
        return -1;
    }
    if (button <= 0 || (button & HORI_CONTROLLER_CONFIG) != HORI_CONTROLLER_CONFIG) {
        return -1;
    }
    struct hori_config_generic_gamepad_report* report = &gamepad_report->config.generic;
    switch (button) {
    case HORI_BUTTON_UP:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_UP) == HORI_DPAD_UP;
    case HORI_BUTTON_DOWN:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_DOWN) == HORI_DPAD_DOWN;
    case HORI_BUTTON_LEFT:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_LEFT) == HORI_DPAD_LEFT;
    case HORI_BUTTON_RIGHT:
        return report->buttons.hat_released == 0 && (hori_playstation_hat[report->buttons.hat_buttons & 7] & HORI_DPAD_RIGHT) == HORI_DPAD_RIGHT;
    case HORI_BUTTON_L1:
        return report->buttons.l1;
    case HORI_BUTTON_R1:
        return report->buttons.r1;
    case HORI_BUTTON_L3:
        return report->buttons.l3;
    case HORI_BUTTON_R3:
        return report->buttons.r3;
    case HORI_BUTTON_FL1:
        return report->buttons.fl1;
    case HORI_BUTTON_FL2:
        // ??
        return -1;
    case HORI_BUTTON_FR1:
        return report->buttons.fr1;
    case HORI_BUTTON_FR2:
        // ??
        return -1;
    case HORI_BUTTON_L2:
        return report->buttons.l2;
    case HORI_BUTTON_R2:
        return report->buttons.r2;
    case HORI_BUTTON_CROSS:
        return report->buttons.cross;
    case HORI_BUTTON_CIRCLE:
        return report->buttons.circle;
    case HORI_BUTTON_SQUARE:
        return report->buttons.square;
    case HORI_BUTTON_TRIANGLE:
        return report->buttons.triangle;
        /*
        HORI_BUTTON_LPEDAL = 19,
        HORI_BUTTON_RPEDAL = 20,
        */
    case HORI_BUTTON_PLAYSTATION:
        return report->buttons.home;
    case HORI_BUTTON_SELECT:
        return report->buttons.real_circle;
    case HORI_BUTTON_START:
        return report->buttons.real_cross;
    case HORI_BUTTON_CAMERA:
        return report->buttons.camera;
        /*       HORI_BUTTON_LSTICK_UP = 25,
    HORI_BUTTON_LSTICK_DOWN = 26,
    HORI_BUTTON_LSTICK_LEFT = 27,
    HORI_BUTTON_LSTICK_RIGHT = 28,

    HORI_BUTTON_RSTICK_UP = 29,
    HORI_BUTTON_RSTICK_DOWN = 30,
    HORI_BUTTON_RSTICK_LEFT = 31,
    HORI_BUTTON_RSTICK_RIGHT = 32,

    HORI_BUTTON_LAGILE = 33,
    HORI_BUTTON_LTARGET = 34,
    */

    }
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
        return hori_get_config_button(&gamepad->report, button);
        //   case HORI_CONTROLLER_HID:
       //        return hori_get_hid_button(&gamepad->report.hid, button);
    }
    return -1;
}

int hori_get_ps4_buttons(struct hori_ps4_gamepad_report* report, int group) {
    if (report == NULL) {
        return -1;
    }
    int button_count = 0;
    int buttons = 0;
    for (int button = 1; button < sizeof(int) * 8 - 1; ++button) {
        int bit_mask = HORI_BUTTON_BIT_MASK(button);
        if (bit_mask <= 0) {
            break;
        }
        int state = hori_get_ps4_button(report, HORI_CONTROLLER_PLAYSTATION4 | button);
        button_count += (state != -1);
        if (state == 1) {
            buttons = buttons | bit_mask;
        }
    }
    if (button_count == 0) {
        return -1;
    }
    return buttons;
}
int hori_get_controller_buttons(union hori_gamepad_report* report, int group, int controller, int (*get_controller_button)(union hori_gamepad_report*, int)) {
    if (report == NULL) {
        return -1;
    }
    if (get_controller_button == NULL) {
        return -1;
    }
    int button_count = 0;
    int buttons = 0;
    for (int button = 1; button < sizeof(int) * 8 - 1; ++button) {
        int bit_mask = HORI_BUTTON_BIT_MASK(button);
        if (bit_mask <= 0) {
            break;
        }
        int state = -1;
        if (controller == HORI_CONTROLLER_CONFIG) {
            state = hori_get_config_button_lookup(report->raw, sizeof(report->raw), hori_config_generic_gamepad_lookup, sizeof(hori_config_generic_gamepad_lookup) / sizeof(hori_config_generic_gamepad_lookup[0]), button);
            //state = get_controller_button(report, controller | button);
        }
        else {
            state = get_controller_button(report, controller | button);
        }
        button_count += (state != -1);
        if (state == 1) {
            buttons = buttons | bit_mask;
        }
    }
    if (button_count == 0) {
        return -1;
    }
    return buttons;
}

int HORI_API_CALL hori_get_buttons(hori_gamepad_t* gamepad, int group) {
    if (gamepad == NULL) {
        return -1;
    }
    if (gamepad->device_state == HORI_STATE_CONFIG) {
        return hori_get_controller_buttons(&gamepad->report, group, HORI_CONTROLLER_CONFIG, &hori_get_config_button);
    }
    switch (gamepad->device_controller) {
    case HORI_CONTROLLER_PLAYSTATION4:
        return hori_get_ps4_buttons(&gamepad->report.ps4, group);
    case HORI_CONTROLLER_CONFIG:
        return hori_get_controller_buttons(&gamepad->report, group, HORI_CONTROLLER_CONFIG, &hori_get_config_button);
    }
    return -1;
}
