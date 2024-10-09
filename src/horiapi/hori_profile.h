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

struct hori_value_config {
    unsigned char enabled;
    unsigned char value;
};

HORI_STATIC_ASSERT(sizeof(struct hori_value_config) == 2, "");

struct hori_analog_config {
    unsigned char orginal;
    unsigned char target;
};

/** @brief Describe stick configuration

    @since 0.1.0
 */
struct hori_stick_config {
    unsigned char any_enabled; // 0 - ?? 
    struct hori_value_config dead_zone_area;
    struct {
        unsigned char enabled;  // 3
        unsigned char vertical; // 4
        unsigned char horizontal; // 5
    } reverse_axis;
    unsigned char stick_reverse; // 6: 1 - left, 2 - right, 3 - wheel
    struct hori_value_config target; // 7 - 8
    struct hori_value_config agile; // 9 - 10 (always 10 if 9 is enabled)
    unsigned char linear;
    struct hori_analog_config analog[2];

};
HORI_STATIC_ASSERT(sizeof(struct hori_stick_config) == 16, "");

enum hori_stick_option {
    HORI_STICK_OPTION_ALL, // - bool
    HORI_STICK_OPTION_KEY_DEAD_ZONE, // deadzone settings
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
    unsigned char Feedback[HORI_FEEDBACK_CONFIG_SIZE];
};
HORI_STATIC_ASSERT(sizeof(struct hori_feedback_config) == HORI_FEEDBACK_CONFIG_SIZE, "");

enum hori_button_option {
    HORI_BUTTON_OPTION_ALL,
    HORI_BUTTON_OPTION_TURBO, // also quick turbo
    HORI_BUTTON_OPTION_DEAD_RANGE,
    HORI_BUTTON_OPTION_EDGE_DEAD_RANGE,
    HORI_BUTTON_OPTION_MAPPED,
    HORI_BUTTON_OPTION_LINEAR_VALUE,
};

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

    unsigned char turbo_enabled;			// 3
    unsigned char turbo_speed;	// 4
    unsigned char quick_turbo_enabled;	//5
    unsigned char dead_range;	//6 
    unsigned char edge_dead_range;//7
    struct hori_value_config map_button; // 8-9
    unsigned char linear_value;	//10
    struct hori_analog_config map_analog[2];			//14
};
_HORI_STATIC_ASSERT(sizeof(struct hori_button_config) == HORI_BUTTON_CONFIG_SIZE , "");

/** @brief Get value of key

    @since 0.1.0
    @param button The button config pointer

    @returns
        This function return -1 on error, or value on success (always in range 0-255)
  */
int hori_internal_get_button_config_value(struct hori_button_config const* button, int key);

int hori_internal_set_button_config_value(struct hori_button_config* button, int key, char value);
int hori_internal_get_button_config_enabled(struct hori_button_config const* button, int key);
int hori_internal_set_button_config_enabled(struct hori_button_config* stick, int key, int value);


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
            char reserve[HORI_PROFILE_SPF023_RESERVE_SIZE]; // 44 - 52
        } spf023;
        struct {
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
