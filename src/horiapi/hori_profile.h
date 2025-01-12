#pragma once
#include "hori_endian.h"
#include "hori_assert.h"

#define HORI_FEEDBACK_CONFIG_SIZE 6
#define HORI_BUTTON_CONFIG_SIZE 14
#define HORI_PROFILE_NAME_SIZE 32
#define HORI_PROFILE_MOTOR_PACKET_SIZE 6
#define HORI_PROFILE_AUDIO_SIZE 5

#define HORI_PROFILE_COMMON_SENSOR_SIZE 13
#define HORI_PROFILE_COMMON_RESERVE_SIZE 0

#define HORI_PROFILE_SPF023_SENSOR_SIZE 4
#define HORI_PROFILE_SPF023_RESERVE_SIZE 9

#define HORI_PROFILE_BUTTONS_COUNT 24
#define HORI_AUDIO_CONFIG_SIZE 5
#define HORI_WHEEL_CONFIG_SIZE 5
#define HORI_PROFILE_CONFIG_SIZE 431

/** @brief Optional value representation
 */
struct hori_value_config {
    unsigned char enabled;
    /** @brief Stored value used only when enabled is set to true */
    unsigned char value;
};

HORI_STATIC_ASSERT(sizeof(struct hori_value_config) == 2, "");

struct hori_analog_config {
    unsigned char movement;
    unsigned char response;
};

/** @brief Describe stick configuration

    @since 0.1.0
 */
struct hori_stick_config {
    /** @brief Function enabled

        Non zero if any of features is enabled:
            HorReverse
            VerticalReverse
            AnyReverse
            TargetSetting
            Agile
            LinearValue
            DeadZone
     */
    unsigned char any_enabled; // 0 - ??
    /** @brief Enable dead zone

        value normalized from 0 to 100
      */
    struct hori_value_config dead_zone_area;
    struct {
        unsigned char enabled;  // 3 - 1 enabled (vertical or horizontal) 0 disabled
        unsigned char vertical; // 4 - 1 enabled 0 disabled 
        unsigned char horizontal; // 5 - 1 enabled 0 disabled
    } reverse_axis;
    unsigned char stick_reverse; // 6: 1 - left, 2 - right, 3 - wheel
    /** @brief setting maximum value of analog stick

        Define maximum value of analog stick that can be set (upper treshold filter)

        Target Value is from range 0 - 100 (it is mapped to 0 255)
     */
    struct hori_value_config target; // 7 - 8
    /** @brief Setting digital mode (step function)

        Basically angular dead zones (for 8 cardinal directions)

        Converting analog to digital value
        When step function is enabled all values after set-point will be set as max value of analog all lower will be sat as 0

        default agile value is 10% (value from range 0-100)?
        */
    struct hori_value_config agile; // 9 - 10 (always 10 if 9 is enabled)
    /** @brief Use linear mapping

        When value is set to 0 linear mapping is not used, otherwise
        values in @analog are defined as:
          analog[0]
          analog[1] = {0, 0}

            function (e, t) {
    const n = "Left",
      i = "Right",
      a = [
        PEDAL IS REVERSE AXIS - MORE IS LESS
        LESS RESPONSIVE
        { Level: 3, Original: 222, Target: 52, PedalDirection: n },
        { Level: 2, Original: 222, Target: 104, PedalDirection: n },
        { Level: 1, Original: 222, Target: 164, PedalDirection: n },
        DEFAULT ? { Level: 0, Original: 222, Target: 222, PedalDirection: n },
        { Level: -1, Original: 222, Target: 215, PedalDirection: n },
        { Level: -2, Original: 222, Target: 210, PedalDirection: n },
        { Level: -3, Original: 222, Target: 205, PedalDirection: n },
        MORE RESPONSIVE

            0 . 2 . 4 . 6 . 8 . 1 . 1 . 1 . 1 . 2 . 2
                0   0   0   0   2   4   6   8   2   4
                                0   0   0   0   0   0
        240            +3      +2            +132  10
        .
        220
        .
        180
        .
        160
        .
        140
        .
        120
        .
        80
        .
        60
        .
        40
        .
        20
        .
        0
            0 . 2 . 4 . 6 . 8 . 1 . 1 . 1 . 1 . 2 . 2
                0   0   0   0   2   4   6   8   2   4
                                0   0   0   0   0   0
        { Level: 3, Original: 240, Target: 60, PedalDirection: i },
        { Level: 2, Original: 240, Target: 120, PedalDirection: i },
        { Level: 1, Original: 240, Target: 200, PedalDirection: i },
        { Level: 0, Original: 240, Target: 240, PedalDirection: i },
        { Level: -1, Original: 240, Target: 230, PedalDirection: i },
        { Level: -2, Original: 240, Target: 220, PedalDirection: i },
        { Level: -3, Original: 240, Target: 210, PedalDirection: i },
      ],
      r = {
          */
    unsigned char linear;
    /** @brief Remap values based on analog range

        Define set of two points p^1 p^2
        disable mapping:
            p^1=(0, 0)
            p^2=(0, 0)
        reverse mapping
            p^0=(0, 255)
            p^1=(255, 0)
        identity mapping
            p^0=(0, 0)
            p^1=(255, 255)
     */
    struct hori_analog_config analog[2];

};
HORI_STATIC_ASSERT(sizeof(struct hori_stick_config) == 16, "");

enum hori_stick_option {
    HORI_STICK_OPTION_ALL, // - bool
    HORI_STICK_OPTION_DEAD_ZONE, // deadzone settings
    HORI_STICK_OPTION_REVERSE_AXIS, // flag true false - bool
    HORI_STICK_OPTION_REVERSE_AXIS_HORIZONTAL, // depends on reverse_axis - bool
    HORI_STICK_OPTION_REVERSE_AXIS_VERTICAL,   // bool
    HORI_STICK_OPTION_MAPPING,
    HORI_OPTION_TARGET,
    HORI_OPTION_AGILE,
};

int hori_internal_set_stick_value(struct hori_stick_config* stick, int key, char value);
int hori_internal_set_stick_flag(struct hori_stick_config* stick, int key, int value);

struct hori_feedback_config {
    unsigned char unknown0;
    unsigned char LeftVibration; // 1
    unsigned char RightVibration; //2
    unsigned char unknown1;
    unsigned char SideLeftVibration; // 4
    unsigned char SideRightVibration; // 5
};
HORI_STATIC_ASSERT(sizeof(struct hori_feedback_config) == HORI_FEEDBACK_CONFIG_SIZE, "");

struct hori_button_config {
    unsigned char enabled;			// 1
    union {
        // depends on low/high endian
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            unsigned char flag_turbo : 1;
            unsigned char flag_dead_zone : 1;
            unsigned char flag_mapping : 1;
            unsigned char flag_linear : 1;
            unsigned char flag_unused : 4;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            unsigned char flag_unused : 4;
            unsigned char flag_linear : 1;
            unsigned char flag_mapping : 1;
            unsigned char flag_dead_zone : 1;
            unsigned char flag_turbo : 1;
#else
#error "TODO"
#endif
        };
        unsigned char flags;
    };// 2

    unsigned char turbo_enabled;			// 3 bool 0 , 1 - if supported 
    unsigned char turbo_speed;	// 4 - 1 turbo (disabled normal) if > 1 then turbo is used when @turbo enabled is set value is set in HZ
    unsigned char quick_turbo_enabled;	//5 always the same as 0, the same as turbo enabled (can be changed)
    unsigned char dead_range;	//6 
    unsigned char edge_dead_range;//7 - alwayes dead_range + 1 (for dpad)
    struct hori_value_config map_button; // 8-9 - equal button by default (indexof button)
    unsigned char linear_analog_enabled;	//10 - probably value of mapped analog button ??
    struct hori_analog_config map_analog[2];			//14 - same thing as in analog value 
};
HORI_STATIC_ASSERT(sizeof(struct hori_button_config) == HORI_BUTTON_CONFIG_SIZE, "");

/** @brief Hori audio configuration */
struct hori_audio_config {
    unsigned char enabled; // 0
    unsigned char volume; // 1
    unsigned char mixer; // from 0 (only first) to 50 (both equal) 100 (only last) 
    unsigned char microphone_mute;
    unsigned char microphone_sensitivity;
};
HORI_STATIC_ASSERT(sizeof(struct hori_audio_config) == HORI_AUDIO_CONFIG_SIZE, "");

struct hori_wheel_config {
    unsigned char enable_0; // always 1 if wheel is supported
    unsigned char enable_1; // always 1 if wheel is supported
    unsigned char range; // e[2]
    unsigned char deadzone; // if e[3] == 0 ? e[3] = 4; e[3] - 4
    unsigned char sensitivity; // if e[4] == 0 ? e[4] = 4; e[4] - 4
};
HORI_STATIC_ASSERT(sizeof(struct hori_wheel_config) == HORI_WHEEL_CONFIG_SIZE, "");

struct hori_profile_config {
    char name[HORI_PROFILE_NAME_SIZE];		// 0 - 31
    // how stick works 
    unsigned char dpad_left_stick_right_stick[2];	// 32 - 33
    struct hori_feedback_config feedback; // 34 - 39
    /** @brief Audio config */
    struct hori_audio_config audio;
    union {
        struct {
            char sensor[HORI_PROFILE_SPF023_SENSOR_SIZE]; // 40 - 43
            //for SPF_023 or PC2161
            // data is (NOT KNOWN WHERE T, R, B, L is stored)
            // 
            // 0 - dpadconfig.TR
            // 1 - dpadconfig.RB
            // 2 - dpadconfig.BL
            // 3 - dpadconfig.LT
            // 4 - balanced input if 0 or 1 or 2 (not balanced input)
            // 5 - 0
            // 6 - 0
            // 7 - 0
            // 8 - 0
            char reserve[HORI_PROFILE_SPF023_RESERVE_SIZE]; // 44 - 52
        } spf023;
        struct {
            /*
            (Tt.Sensor.SensorEnabled = m(e[0], 0)),
            (Tt.Sensor.QuickKeyEnabled = m(e[0], 1)),
            (Tt.Sensor.QuickAxisType = m(e[0], 2) ? 1 : 0),
            (Tt.Sensor.Sensor_switch = e[1]),
            (Tt.Sensor.Sensitivity = e[3]),
            (Tt.Sensor.Original_data1 = e[5]),
            (Tt.Sensor.Target_data1 = e[6]),
            (Tt.Sensor.Original_data2 = e[7]),
            (Tt.Sensor.Target_data2 = e[8]),
            (Tt.Sensor.SensorAsLs = M(e[9])),
            (Tt.Sensor.HorReverse = M(e[10])),
            (Tt.Sensor.VerReverse = M(e[11])),
            (Tt.Sensor.QuickKey = It.getButtonKey(e[12])),

            */
            char sensor[HORI_PROFILE_COMMON_SENSOR_SIZE]; // 40 - 43
        } common;
    };
    union hori_profile_mapping {
        struct {
            struct hori_button_config button_up; // 53 (14 per button) - 388
            struct hori_button_config button_down;
            struct hori_button_config button_left;
            struct hori_button_config button_right;

            struct hori_button_config button_l1;
            struct hori_button_config button_r1;
            struct hori_button_config button_l3;
            struct hori_button_config butten_r3;

            struct hori_button_config button_fl1;
            struct hori_button_config button_fl2;
            struct hori_button_config button_fr1;
            struct hori_button_config button_fr2;

            struct hori_button_config button_l2;
            struct hori_button_config button_r2;

            struct hori_button_config button_cross;
            struct hori_button_config button_circle;
            struct hori_button_config button_square;
            struct hori_button_config button_triangle;

            struct hori_button_config button_lpedal;
            struct hori_button_config button_rpedal;

            struct hori_button_config button_playstation;
            struct hori_button_config button_select;
            struct hori_button_config button_start;
            struct hori_button_config button_camera;
        };
        struct hori_button_config buttons[HORI_PROFILE_BUTTONS_COUNT];
    };
    /** @brief Left stick configuration */
    struct hori_stick_config left_stick;
    /** @brief Right stick configuration */
    struct hori_stick_config right_stick;
    /** @brief Wheel configuration */
    struct hori_wheel_config wheel;
};

HORI_STATIC_ASSERT(sizeof(union hori_profile_mapping) == HORI_PROFILE_BUTTONS_COUNT * sizeof(struct hori_button_config), "");
HORI_STATIC_ASSERT(sizeof(struct hori_profile_config) == HORI_PROFILE_CONFIG_SIZE, "");

/** @brief Check if profile config is valid

    @detials
        Profile is valid if:
            - name contains valid UTF-8 string and rest of the bytes are filled with 0

    @since 0.1.0
    @param config Pointer to @see hori_profile_config

    @returns
        This function returns -1 if profile is invalid and 0 otherwise

    @todo check other fields for known values (i.e. hori_button_config)
 */
int hori_internal_is_valid_profile_config(struct hori_profile_config* config);

#define HORI_PROFILE_NO_ERROR 0

enum hori_profile_error_code {
    // Unknown error
    HORI_PROFILE_ERROR_CODE_UNKNOWN = 1
};

/** @brief Describe device profile
 */
struct hori_profile {
    /** @brief Equal to @see HORI_API_VERSION */
    int hori_api_version;
    /** @brief Always zero terminated profile name */
    char name[HORI_PROFILE_NAME_SIZE + 1];
    /** @brief Product id */
    int product;
    /** @brief Last error code */
    int error_code; // 0 meens no error
    /** @brief Profile config */
    struct hori_profile_config config;
};
