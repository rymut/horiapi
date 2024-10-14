#pragma once

#include <inttypes.h>

#include "horiapi/horiapi.h"

#include "hori_endian.h"
#include "hori_assert.h"
#include "hori_alignof.h"

struct hori_linear_value {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned char positive : 1;
    unsigned char value : 7;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned char value : 7;
    unsigned char positive : 1;
#endif
};
HORI_STATIC_ASSERT(sizeof(struct hori_linear_value) == 1, "");
HORI_STATIC_ASSERT(HORI_ALIGNOF(struct hori_linear_value) == 1, "");

struct hori_stick_value {
    struct hori_linear_value x;
    struct hori_linear_value y;
};
HORI_STATIC_ASSERT(HORI_ALIGNOF(struct hori_stick_value) == 1, "");

// xbox descriptior https://github.com/nefarius/ViGEmBus/issues/40
// https://github.com/DJm00n/ControllersInfo/blob/master/dualsense/dualsense_hid_report_descriptor.txt - best resource
// https://github.com/nondebug/dualsense
struct hori_ps5_gamepad_report {
    unsigned char report_id;
};

// https://github.com/JibbSmart/JoyShockLibrary/blob/master/README.md
// https://www.psdevwiki.com/ps4/DS4-USB
// https://controllers.fandom.com/wiki/Sony_DualShock_4/Data_Structures
struct hori_ps4_touch_finger_data {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint8_t index : 7;
    uint8_t no_touch : 1;
    uint16_t x : 12;
    uint16_t y : 12;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint16_t y : 12;
    uint16_t x : 12;
    uint8_t no_touch : 1;
    uint8_t index : 7;
#endif
};

HORI_STATIC_ASSERT(HORI_ALIGNOF(struct hori_ps4_touch_finger_data) == 1, "");

struct hori_ps4_touch_data {
    uint8_t timestamp;
    struct hori_ps4_touch_finger_data finger[2];
};

struct hori_ps4_extra_data {
    uint16_t timestamp; // in 5.33us units?
    uint8_t temperature;
    int16_t angular_velocity_x;
    int16_t angular_velocity_y;
    int16_t angular_velocity_z;
    int16_t accelerometer_x;
    int16_t accelerometer_y;
    int16_t accelerometer_z;
    uint8_t ext_data[5]; // range can be set by EXT device
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint8_t power_percent : 4; // 0x00-0x0A or 0x01-0x0B if plugged int
    uint8_t plugged_power_cable : 1;
    uint8_t plugged_headphones : 1;
    uint8_t plugged_microphone : 1;
    uint8_t plugged_ext : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t plugged_ext : 1;
    uint8_t plugged_microphone : 1;
    uint8_t plugged_headphones : 1;
    uint8_t plugged_power_cable : 1;
    uint8_t power_percent : 4; // 0x00-0x0A or 0x01-0x0B if plugged int
#else
#error "not known"
#endif 
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint8_t unknown_ext1 : 1; // ExtCapableOfExtraData?
    uint8_t unknown_ext2 : 1; // ExtHasExtraData?
    uint8_t not_connected : 1; // Used by dongle to indicate no controller
    uint8_t Unk1 : 5;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t Unk1 : 5;
    uint8_t not_connected : 1; // Used by dongle to indicate no controller
    uint8_t unknown_ext2 : 1; // ExtHasExtraData?
    uint8_t unknown_ext1 : 1; // ExtCapableOfExtraData?
#else
#error "not known"
#endif 
    uint8_t Unk2; // unused?
    uint8_t touch_count;
};
/** @brief PS4 buttons structure
 */
struct hori_ps4_gamepad_report {
    /* -1 */ unsigned char report_id;
    unsigned char left_stick_x;
    unsigned char left_stick_y;
    unsigned char right_stick_x;
    unsigned char right_stick_y;
    struct hori_ps4_buttons {
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char hat_buttons : 3;
            unsigned char hat_released : 1;
            unsigned char square : 1;
            unsigned char cross : 1;
            unsigned char circle : 1;
            unsigned char triangle : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char triangle : 1;
            unsigned char circle : 1;
            unsigned char cross : 1;
            unsigned char square : 1;
            unsigned char hat_released : 1;
            unsigned char hat_buttons : 3;
#endif 
        };
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char l1 : 1;
            unsigned char r1 : 1;
            unsigned char l2 : 1;
            unsigned char r2 : 1;
            unsigned char share : 1;
            unsigned char options : 1;
            unsigned char l3 : 1;
            unsigned char r3 : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char r3 : 1;
            unsigned char l3 : 1;
            unsigned char options : 1;
            unsigned char share : 1;
            unsigned char r2 : 1;
            unsigned char l2 : 1;
            unsigned char r1 : 1;
            unsigned char l1 : 1;
#endif
        };
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char ps : 1;
            unsigned char tpad : 1;
            unsigned char counter : 6;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char counter : 6;
            unsigned char tpad : 1;
            unsigned char ps : 1;
#endif
        };
    } buttons;
    unsigned char left_trigger; // l2 analog
    unsigned char right_trigger; // r2 analog
    struct hori_ps4_extra_data extra;
    struct hori_ps4_touch_data touch;
    struct hori_ps4_touch_data touch_ext[];
};

struct hori_hpc043_wheel_gamepad_report {
    unsigned char report_id;
};
// see https://hori.co.uk/hpc-043u/mapping
struct hori_hpc043_panel_gamepad_report {
    unsigned char report_id;    // 0
    unsigned char left_stick_x; // 1
    unsigned char left_stick_y; // 2
    char stick_rool; // 3 as character (number)
    struct {
        unsigned char key1 : 1;
        unsigned char key2 : 1;
        unsigned char key3 : 1;
        unsigned char key4 : 1;
        unsigned char key5 : 1;
        unsigned char key6 : 1;
        unsigned char unknown : 2;
    };

};

/** @brief Describe hori gamepad report when controller is in HORI_STATE_CONFIG state
 */
struct hori_config_generic_gamepad_report {
    /** @brief HID report id */
    unsigned char report_id; // 0
    struct hori_stick_value left_stick; // 1
    struct hori_stick_value right_stick; // 3
    struct hori_config_buttons {
        // offset 5
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char hat_buttons : 3;
            unsigned char hat_released : 1;
            unsigned char square : 1;
            unsigned char cross : 1;
            unsigned char circle : 1;
            unsigned char triangle : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char triangle : 1;
            unsigned char circle : 1;
            unsigned char cross : 1;
            unsigned char square : 1;
            unsigned char hat_released : 1;
            unsigned char hat_buttons : 3;
#endif 
        };
        // offset 6
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char l1 : 1;
            unsigned char r1 : 1;
            unsigned char l2 : 1;
            unsigned char r2 : 1;
            unsigned char share : 1;
            unsigned char options : 1;
            unsigned char l3 : 1;
            unsigned char r3 : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char r3 : 1;
            unsigned char l3 : 1;
            unsigned char options : 1;
            unsigned char share : 1;
            unsigned char r2 : 1;
            unsigned char l2 : 1;
            unsigned char r1 : 1;
            unsigned char l1 : 1;
#endif
        };
        // offset 7
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char camera : 1;
            unsigned char home : 1;
            unsigned char fr1 : 1;
            unsigned char fl1 : 1;
            unsigned char unused4 : 4;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char unused4 : 4;
            unsigned char fl1 : 1;
            unsigned char fr1 : 1;
            unsigned char home : 1;
            unsigned char camera : 1;
#endif
        };
        // offset 8
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char real_circle : 1;
            unsigned char real_cross : 1;
            unsigned char unused6 : 6;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char unused6 : 6;
            unsigned char real_cross : 1;
            unsigned char real_circle : 1;
#endif
        };
    } buttons;
    // offset 9
    struct hori_linear_value left_trigger;			// 1 Rx
    // offset 10
    struct hori_linear_value right_trigger;			// 1 Ry 
    // offset 11
    struct hori_ps4_extra_data extra;
    struct hori_ps4_touch_data touch;
    // offset 43
    unsigned short wheel;		// 2 // le
    // offset 44
    unsigned short left_pedal;	// 2 // 65535 - le
    // offset 46
    unsigned short right_pedal;	// 2 // 65535 - le
    // offset 48
};

/** @brief Describe hori gamepad report when controller is in HORI_STATE_CONFIG state
 */
struct hori_config_spf023_gamepad_report {
    /** @brief HID report id */
    unsigned char report_id;
    struct hori_stick_value left_stick;		// 2 - x/y
    struct hori_stick_value right_stick;		// 2 - z/rz
    struct {
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char hat_buttons : 3;
            unsigned char hat_released : 1;
            unsigned char square : 1;
            unsigned char cross : 1;
            unsigned char circle : 1;
            unsigned char triangle : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char triangle : 1;
            unsigned char circle : 1;
            unsigned char cross : 1;
            unsigned char square : 1;
            unsigned char hat_released : 1;
            unsigned char hat_buttons : 3;
#endif 
        };
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char l1 : 1;
            unsigned char r1 : 1;
            unsigned char l2 : 1;
            unsigned char r2 : 1;
            unsigned char share : 1;
            unsigned char options : 1;
            unsigned char l3 : 1;
            unsigned char r3 : 1;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char r3 : 1;
            unsigned char l3 : 1;
            unsigned char options : 1;
            unsigned char share : 1;
            unsigned char r2 : 1;
            unsigned char l2 : 1;
            unsigned char r1 : 1;
            unsigned char l1 : 1;
#endif
        };
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char camera : 1;
            unsigned char home : 1;
            unsigned char fr1 : 1;
            unsigned char fl1 : 1;
            unsigned char unused4 : 4;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char unused4 : 4;
            unsigned char fl1 : 1;
            unsigned char fr1 : 1;
            unsigned char home : 1;
            unsigned char camera : 1;
#endif
        };
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char real_circle : 1;
            unsigned char real_cross : 1;
            unsigned char unused6 : 6;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char unused6 : 6;
            unsigned char real_cross : 1;
            unsigned char real_circle : 1;
#endif
        };
    } buttons;
    // offset 8
    struct hori_linear_value left_trigger;			// 1 Rx
    struct hori_linear_value right_trigger;			// 1 Ry 
    // offset 10
    struct hori_ps4_extra_data extra;
    struct hori_ps4_touch_data touch;
    // offset 42
    unsigned short wheel;		// 2 // le
    unsigned short left_pedal;	// 2 // 65535 - le
    unsigned short right_pedal;	// 2 // 65535 - le
    // offset 48
};

struct hori_xinput_gamepad_report {
    unsigned char report_id;
};

struct hori_gamepad {
    // bit mapping for buttons
    int device_product;
    int device_controller;
    int device_state;
    int button_map[34]; // ?? how any buttons here
    union hori_gamepad_report {
        struct hori_xinput_gamepad_report xinput;
        union {
            struct hori_config_generic_gamepad_report generic;
            struct hori_config_spf023_gamepad_report spf023;
        } config;
        struct hori_ps4_gamepad_report ps4;
        struct hori_ps5_gamepad_report ps5;
        uint8_t raw[64];
    } report;
};

/** @brief Get button status

    @returns
        This function returns 0 if button is not pressed, 1 if button is pressed, -1 on error

    @note
        Analog button values are discretized to 0, 1 (not zero value)
 */
int hori_get_gamepad_button(struct hori_gamepad* gamepad, int button);

/** @brief Get hat values
 */
int hori_get_gamepad_dpad(struct hori_gamepad* gamepad);

enum hori_axis {
    HORI_AXIS_X,
    HORI_AXIS_Y,
    HORI_AXIS_Z,
    HORI_AXIS_RX,
    HORI_AXIS_RY,
    HORI_AXIS_RZ,
    HORI_AXIS_LEFT_Z, // JAK OPISAC OSIE 

    HORI_ANALOG_Y_AXIS,
    HORI_ANALOG_Z_AXIS,
    HORI_ANALOG_X_ROTATION,
    HORI_ANALOG_Y_ROTATION,
};
/** @brief Get axis

    @note
        DPAD values are matched to axis 0-255
 */
int hori_get_gamepad_axis(struct hori_gamepad* gamepad, int axis);

