#pragma once

#include <inttypes.h>

struct hori_linear_value {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned char positive : 1;
    unsigned char value : 7;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned char value : 7;
    unsigned char positive : 1;
#endif
};

struct hori_stick_value {
    struct hori_linear_value x;
    struct hori_linear_value y;
};

// https://www.psdevwiki.com/ps4/DS4-USB
/** @brief PS4 buttons structure
 */
struct hori_ps4_gamepad_report {
    /* -1 */ unsigned char report_id;
    struct hori_stick_value left_stick;
    struct hori_stick_value right_stick;
    struct {
        unsigned char triangle : 1;
        unsigned char circle : 1;
        unsigned char cross : 1;
        unsigned char square : 1;
        unsigned char hat_center : 1;
        unsigned char hat_buttons : 3;
    };
    struct {
        unsigned char r3 : 1;
        unsigned char l3 : 1;
        unsigned char options : 1;
        unsigned char share : 1;
        unsigned char r2 : 1;
        unsigned char l2 : 1;
        unsigned char r2 : 1;
        unsigned char l1 : 1;
    };
    struct {
        unsigned char counter: 6;
        unsigned char tpad : 1;
        unsigned char ps : 1;
    };
    unsigned char l2; // analog
    unsigned char r2; // analog
    /* 9  */ uint16_t timestamp; // in 5.33us units?
    /*11  */ uint8_t battery;
    /*12  */ int16_t angular_velocity_x;
    /*14  */ int16_t angular_velocity_y;
    /*16  */ int16_t angular_velocity_z;
    /*18  */ int16_t accelerometer_x;
    /*20  */ int16_t accelerometer_y;
    /*22  */ int16_t accelerometer_y;
    /*24  */ uint8_t ext_data[5]; // range can be set by EXT device
    /*29  */ uint8_t power_percent: 4; // 0x00-0x0A or 0x01-0x0B if plugged int
    /*29.4*/ uint8_t plugged_power_cable : 1;
    /*29.5*/ uint8_t plugged_headphones : 1;
    /*29.6*/ uint8_t plugged_microphone : 1;
    /*29,7*/ uint8_t plugged_ext: 1;
    /*30.0*/ uint8_t unknown_ext1: 1; // ExtCapableOfExtraData?
    /*30.1*/ uint8_t unknown_ext2: 1; // ExtHasExtraData?
    /*30.2*/ uint8_t NotConnected : 1; // Used by dongle to indicate no controller
    /*30.3*/ uint8_t Unk1 : 5;
    /*31  */ uint8_t Unk2; // unused?
    /*32  */ uint8_t TouchCount;
};

};

struct hori_buttons {
    // depends on controller mapping
    struct {
        unsigned char Right3 : 1;
        unsigned char Left3 : 1;
        unsigned char Options : 1;
        unsigned char Share : 1;
        unsigned char Right2 : 1;
        unsigned char Left2 : 1;
        unsigned char Right1 : 1;
        unsigned char Left1 : 1;
    };
    struct {
        unsigned char FireRight1 : 1; // trigger right
        unsigned char FireLeft1 : 1;	// trigger left
        unsigned char Camera : 1;
        unsigned char Home : 1;
        // other models
        unsigned char Other1 : 4;
    };
    unsigned char Other2;
};
static_assert(sizeof(AnalogStick) == 2, "correct assumptions");
static_assert(sizeof(Buttons) == 4, "correct assumptions");

struct hori_gamepad_report {
    struct hori_analog_value LeftStick;		// 2 - x/y
    struct hori_analog_value RightStick;		// 2 - z/rz
    struct hori_buttons Buttons;			// 4
    // offset 8
    struct hori_linear_value TriggerLeft;			// 1 Rx
    struct hori_linear_value TriggerRight;			// 1 Ry 
    // offset 10
    unsigned char Unknown1[24];
    // offset 34
    unsigned char Touch0[4];	// 4
    unsigned char Touch1[4];	// 4
    // offset 42
    unsigned short Wheel;		// 2
    unsigned short LeftPedal;	// 2
    unsigned short RightPedal;	// 2  = 14 
    // offset 48
};

struct hori_gamepad {
    // bit mapping for buttons
    int button_map[34]; // ?? how any buttons here 
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
    HORI_AXIS_LEFT_X,
    HORI_AXIS_LEFT_Y,
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

