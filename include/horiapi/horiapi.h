#pragma once

#include <inttypes.h>

#define HORI_API_CALL 

#define HORI_API_VERSION_MAJOR 0
#define HORI_API_VERSION_MINOR 1
#define HORI_API_VERSION_PATCH 0

/** @brief Compile-time HORI vendor id */
#define HORI_HID_VENDOR_ID 0x0F0D

/** @brief Generate compile-time version number of HORI API library

    @summary Encoded as <4 bits syntax changes><8 bit major><12 bit minor><8 bit patch>.

    @since 0.1.0

    Can be used to compile-time check library version:
    @code{.c}
    #if HORI_API_VERSION >= HORI_API_MAKE_VERSION(0, 1, 0)
    @endcode

    @ingroup API
*/
#define HORI_API_MAKE_VERSION(mj, mn, p) (((mj) << 20) | ((mn & 0xFFF) << 8) | (p))

/** @brief Complie time version number

    @since 0.1.0

    @see @ref HORI_API_MAKE_VERSION.

    @ingroup API
*/
#define HORI_API_VERSION HORI_API_MAKE_VERSION(HORI_API_VERSION_MAJOR, HORI_API_VERSION_MINOR, HORI_API_VERSION_PATCH)

/* Helper macros */
#if !defined(HORI_DOXYGEN)
#define HORI_API_AS_STR_IMPL(x) #x
#define HORI_API_AS_STR(x) HORI_API_AS_STR_IMPL(x)
#define HORI_API_TO_VERSION_STR(v1, v2, v3) HORI_API_AS_STR(v1.v2.v3)
#endif // !defined(HORI_DOXYGEN)

/** @brief Compile-time c-string version of the library.

    @summary Stored as "<major>.<minor>.<patch>"

    @ingroup API
*/
#define HORI_API_VERSION_STR HORI_API_TO_VERSION_STR(HORI_API_VERSION_MAJOR, HORI_API_VERSION_MINOR, HORI_API_VERSION_PATCH)


/** @brief Get button index *
 */
#define HORI_BUTTON_INDEX(button) (button > 0 ? (button & 0xFF) : 0)

 /** @brief Get button bit index
  */
#define HORI_BUTTON_BIT_INDEX(button) ((button > 0 ? (HORI_BUTTON_INDEX(button)%(sizeof(int)*8-1))-1 : 0xFF) & 0xFF)

  /** @brief Get button bit mask
   */
#define HORI_BUTTON_BIT_MASK(button) (HORI_BUTTON_BIT_INDEX(button)>=(sizeof(int)*8-1)?-1:(1<<HORI_BUTTON_BIT_INDEX(button)))

#ifdef __cplusplus
extern "C" {
#endif

    /** Hori products

        @since 0.1.0
      */
    enum hori_product {
        HORI_PRODUCT_ANY = 0, // hori device but might be unsupported
        HORI_PRODUCT_SPF_021, // profile, mapping
        HORI_PRODUCT_SPF_004, // profile, mapping, wheel,podels 
        HORI_PRODUCT_SPF_022, // profil, key, wheel, pedals
        HORI_PRODUCT_SPF_023, // profile, mapping, dpad
        HORI_PRODUCT_NSW326,  // profile, mapping, left stick, right stick, sensor
        HORI_PRODUCT_SD2112, // profil, mapping, wheel, pedals
        HORI_PRODUCT_SD2113,
        HORI_PRODUCT_SD2115,
        HORI_PRODUCT_HPC043_WHEEL, // wheel, pedals
        HORI_PRODUCT_HPC043_PANEL, // wheel
        HORI_PRODUCT_PC2161 // profile, mapping, dpad
    };
    typedef enum hori_product hori_product_t;

    /** Hori controller state

        @ingroup API
        @since 0.1.0
     */
    enum hori_state {
        HORI_STATE_NONE = -1,
        HORI_STATE_CONFIG = 1,// HORI_CONTROLER_CONFIG
        HORI_STATE_NORMAL = 2 // HORI_CONTROLLER_XINPUT | HORI_CONTROLLER_PS4 | PS5 
    };

    /** @brief Controller support

        @ingroup API
        @since 0.1.0
      */
    enum hori_controller {
        HORI_CONTROLLER_HID = 0x000,			// HID option (BUTTON NUMBER in order)
        HORI_CONTROLLER_CONFIG = 0x000,			// HORI numbers configuration option
        HORI_CONTROLLER_XINPUT = 0x200,		    // windows mode
        HORI_CONTROLLER_PLAYSTATION4 = 0x400,	// playstation 4 mode
        HORI_CONTROLLER_PLAYSTATION5 = 0x800,	// playstation 5 mode
    };

    /** @brief Controller features

        @ingroup API
        @since 0.1.0
      */
    enum hori_feature {
        /** @brief Controller supports mapping */
        HORI_FEATURE_MAPPING = 1,
        /** @brief Controller supports motor */
        HORI_FEATURE_MOTOR = 2,
        /** @brief Controller supports sensors */
        HORI_FEATURE_SENSOR = 4,
        /** @brief Controller supports wheel */
        HORI_FEATURE_PEDALS = 8,
        /** @brief Controller supports left stick */
        HORI_FEATURE_LEFT_STICK = 16,
        /** @brief Controller supports right stick */
        HORI_FEATURE_RIGHT_STICK = 32,
        /** @brief Controller support dpad */
        HORI_FEATURE_DIRECTIONAL_PAD = 64,
        /** @brief Controller support for wheel */
        HORI_FEATURE_WHEEL = 128, 
    };

    /** @brief Hori stick definitions

        @ingroup API
        @since 0.1.0
      */
    enum hori_stick {
        HORI_STICK_LEFT = 1,
        HORI_STICK_RIGHT = 2,
        HORI_STICK_WHEEL = 3
    };

    /** @brief Hori turbo setting */
    enum hori_turbo {
        HORI_TURBO_NONE = 0,
    };

    enum hori_xinput_button {
        HORI_XINPUT_BUTTON_UP = HORI_CONTROLLER_XINPUT | 1,
        HORI_XINPUT_BUTTON_DOWN = HORI_CONTROLLER_XINPUT | 2,
        HORI_XINPUT_BUTTON_LEFT = HORI_CONTROLLER_XINPUT | 3,
        HORI_XINPUT_BUTTON_RIGHT = HORI_CONTROLLER_XINPUT | 4,

        HORI_XINPUT_BUTTON_BACK = HORI_CONTROLLER_XINPUT | 5,
        HORI_XINPUT_BUTTON_GUIDE = HORI_CONTROLLER_XINPUT | 6,
        HORI_XINPUT_BUTTON_HOME = HORI_XINPUT_BUTTON_GUIDE,
        HORI_XINPUT_BUTTON_OPTIONS = HORI_CONTROLLER_XINPUT | 7,

        HORI_XINPUT_BUTTON_A = HORI_CONTROLLER_XINPUT | 8,
        HORI_XINPUT_BUTTON_B = HORI_CONTROLLER_XINPUT | 9,
        HORI_XINPUT_BUTTON_Y = HORI_CONTROLLER_XINPUT | 10,
        HORI_XINPUT_BUTTON_X = HORI_CONTROLLER_XINPUT | 11,

        HORI_XINPUT_BUTTON_RSTICK_UP = HORI_CONTROLLER_XINPUT | 12,
        HORI_XINPUT_BUTTON_RSTICK_DOWN = HORI_CONTROLLER_XINPUT | 13,
        HORI_XINPUT_BUTTON_RSTICK_LEFT = HORI_CONTROLLER_XINPUT | 14,
        HORI_XINPUT_BUTTON_RSTICK_RIGHT = HORI_CONTROLLER_XINPUT | 15,
        HORI_XINPUT_BUTTON_RSTICK = HORI_CONTROLLER_XINPUT | 16,

        HORI_XINPUT_BUTTON_LSTICK_UP = HORI_CONTROLLER_XINPUT | 17,
        HORI_XINPUT_BUTTON_LSTICK_DOWN = HORI_CONTROLLER_XINPUT | 18,
        HORI_XINPUT_BUTTON_LSTICK_LEFT = HORI_CONTROLLER_XINPUT | 19,
        HORI_XINPUT_BUTTON_LSTICK_RIGHT = HORI_CONTROLLER_XINPUT | 20,
        HORI_XINPUT_BUTTON_LSTICK = HORI_CONTROLLER_XINPUT | 21,

        HORI_XINPUT_BUTTON_RBUMPER = HORI_CONTROLLER_XINPUT | 22,
        HORI_XINPUT_BUTTON_RTRIGGER = HORI_CONTROLLER_XINPUT | 23,

        HORI_XINPUT_BUTTON_LBUMPER = HORI_CONTROLLER_XINPUT | 24,
        HORI_XINPUT_BUTTON_LTRIGGER = HORI_CONTROLLER_XINPUT | 25,

    };

    // https://www.playstation.com/en-us/support/hardware/ps5-button-functions/
    enum hori_playstation_button {
        HORI_PLAYSTATION_BUTTON_UP = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 1,
        HORI_PLAYSTATION_BUTTON_DOWN = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 2,
        HORI_PLAYSTATION_BUTTON_LEFT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 3,
        HORI_PLAYSTATION_BUTTON_RIGHT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 4,

        HORI_PLAYSTATION_BUTTON_CREATE = HORI_CONTROLLER_PLAYSTATION5 | 5, // options ps5
        HORI_PLAYSTATION_BUTTON_SHARE = HORI_CONTROLLER_PLAYSTATION4 | 5, // share (ps4)
        HORI_PLAYSTATION_BUTTON_TOUCH_PAD = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 6,
        HORI_PLAYSTATION_BUTTON_OPTIONS = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 7,

        HORI_PLAYSTATION_BUTTON_CROSS = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 8,
        HORI_PLAYSTATION_BUTTON_CIRCLE = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 9,
        HORI_PLAYSTATION_BUTTON_TRIANGLE = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 10,
        HORI_PLAYSTATION_BUTTON_SQUARE = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 11,

        HORI_PLAYSTATION_BUTTON_RSTICK_UP = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 12,
        HORI_PLAYSTATION_BUTTON_RSTICK_DOWN = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 13,
        HORI_PLAYSTATION_BUTTON_RSTICK_LEFT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 14,
        HORI_PLAYSTATION_BUTTON_RSTICK_RIGHT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 15,
        HORI_PLAYSTATION_BUTTON_R3 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 16,

        HORI_PLAYSTATION_BUTTON_PLAYSTATION_PS = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 17,
        HORI_PLAYSTATION_BUTTON_PLAYSTATION_MUTE = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 18,

        HORI_PLAYSTATION_BUTTON_LSTICK_UP = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 19,
        HORI_PLAYSTATION_BUTTON_LSTICK_DOWN = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 20,
        HORI_PLAYSTATION_BUTTON_LSTICK_LEFT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 21,
        HORI_PLAYSTATION_BUTTON_LSTICK_RIGHT = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 22,
        HORI_PLAYSTATION_BUTTON_L3 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 23,

        HORI_PLAYSTATION_BUTTON_R1 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 24,
        HORI_PLAYSTATION_BUTTON_R2 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 25,

        HORI_PLAYSTATION_BUTTON_L1 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 26,
        HORI_PLAYSTATION_BUTTON_L2 = HORI_CONTROLLER_PLAYSTATION4 | HORI_CONTROLLER_PLAYSTATION5 | 27,


    };
    /** @brief Hori button used for mapping */
    enum hori_button {
        HORI_BUTTON_UP = 1,
        HORI_BUTTON_DOWN = 2,
        HORI_BUTTON_LEFT = 3,
        HORI_BUTTON_RIGHT = 4,

        HORI_BUTTON_L1 = 5,
        HORI_BUTTON_R1 = 6,
        HORI_BUTTON_L3 = 7,
        HORI_BUTTON_R3 = 8,

        HORI_BUTTON_FL1 = 9,
        HORI_BUTTON_FL2 = 10,
        HORI_BUTTON_FR1 = 11,
        HORI_BUTTON_FR2 = 12,

        HORI_BUTTON_L2 = 13,
        HORI_BUTTON_R2 = 14,

        HORI_BUTTON_CROSS = 15,
        HORI_BUTTON_CIRCLE = 16,
        HORI_BUTTON_SQUARE = 17,
        HORI_BUTTON_TRIANGLE = 18,

        HORI_BUTTON_LPEDAL = 19,
        HORI_BUTTON_RPEDAL = 20,

        HORI_BUTTON_PLAYSTATION = 21,
        HORI_BUTTON_SELECT = 22,
        HORI_BUTTON_START = 23,
        HORI_BUTTON_CAMERA = 24,

        HORI_BUTTON_LSTICK_UP = 25,
        HORI_BUTTON_LSTICK_DOWN = 26,
        HORI_BUTTON_LSTICK_LEFT = 27,
        HORI_BUTTON_LSTICK_RIGHT = 28,

        HORI_BUTTON_RSTICK_UP = 29,
        HORI_BUTTON_RSTICK_DOWN = 30,
        HORI_BUTTON_RSTICK_LEFT = 31,
        HORI_BUTTON_RSTICK_RIGHT = 32,

        HORI_BUTTON_LAGILE = 33,
        HORI_BUTTON_LTARGET = 34,

        HORI_BUTTON_DISABLED = 255
    };

    /** @brief Structure describe software semver

        @ingroup API
        @since 0.1.0
     */
    struct hori_software_version {
        /** @brief Major version */
        int major;
        /** @brief Minor version */
        int minor;
        /** @brief Patch version */
        int patch;
    };

    /** @brief Structure describe firmware version

        @ingroup API
        @since 0.1.0
     */
    struct hori_firmware_version {
        /** @brief Hardware revision 0 for first revision */
        int hardware_revision;
        /** @brief Software version */
        struct hori_software_version software_version;
    };

    struct hori_device_firmware_config {
        void* data;
    };
    typedef struct hori_device_firmware_config hori_device_firmware_config_t;

    /** @brief Structure used to describe supported devices

        @ingroup API
        @since 0.1.0
     */
    struct hori_device_config {
        /** @brief Firmware name (in firmware) */
        char firmware_name[16];
        /** @brief Product type */
        hori_product_t product : 16;
        /** @brief Device config mode or auto */
        enum hori_controller device_config_mode : 16;
        /** @brief Normal mode HID device product id */
        unsigned short hid_normal_product_id;
        /** @brief Config mode HID device product id */
        unsigned short hid_config_product_id;
        /** @brief Normal operation gamepad HID usage page */
        unsigned short hid_normal_usage_page_gamepad;
        /** @brief Normal operation control HID usage page */
        unsigned short hid_normal_usage_page_control;
        /** @brief Config operation gamepad HID usage page */
        unsigned short hid_config_usage_page_gamepad;
        /** @brief Config operation profile HID usage page */
        unsigned short hid_config_usage_page_profile;
        /** @brief Next device config structure */
        struct hori_device_config* next;
    };
    typedef struct hori_device_config hori_device_config_t;

    /** @brief Context settings */
    struct hori_context {
        /** @brief supported device list or NULL */
        hori_device_config_t* devices;
        /** @brief timeout between device rediscover */
        int rediscover_miliseconds_timeout;
        /** @brief delay between rediscover attempts */
        int rediscover_miliseconds_delay;
        /** @brief read timeout */
        int read_timeout_ms;
        /** @brief read retry timeout */
        int retry_read_timeout_ms;
        /** @brief number of retry attempts */
        int retry_attempts;
    };
    typedef struct hori_context hori_context_t;

    hori_context_t* hori_context();

    // forward delcarations
    typedef struct hori_device hori_device_t;

    struct hori_device_info {
        hori_device_config_t* device_config;
        struct hori_device_info* next;
    };
    typedef struct hori_device_info hori_device_info_t;

    struct hori_enumeration {
        char* path;
        int state;
        wchar_t* manufacturer_string;
        wchar_t* product_string;
        hori_device_config_t* device_config;
        struct hori_enumeration* next;
    };
    typedef struct hori_enumeration hori_enumeration_t;

    // set custom device config
    // get defalut context
    hori_context_t* hori_context();

    /** @brief Enumerate available HORI devices

        @ingroup API
        @since 0.1.0
        @param product The product type to enumerate
        @param context The context used to enumerate devices

        @returns
            This function returns NULL if no device is found or pointer to first item
     */
    hori_enumeration_t* hori_enumerate(hori_product_t product, hori_context_t* context);

    /** @brief Free enumeration

        @param enumeration The enumeration handle returned by @see hori_enumerate
      */
    void hori_free_enumerate(hori_enumeration_t* enumeration);

    /** @brief Open hori device using device path

        @ingroup API
        @since 0.1.0
        @param path Device path
        @param context Device context or NULL

        @returns
            This function returns device handle or NULL if device cannot be open
     */
    hori_device_t* hori_open_path(char* path, hori_context_t* context);

    /** @brief Send heartbeat to device

        @ingroup API
        @since 0.1.0

        @param[in] device - device to write heartbeat

        @returns
            This function returns
     */
    int hori_send_heartbeat(hori_device_t* device);

    /** @brief Get firmware version

        @ingroup API
        @since 0.1.0

        @returns
            This function returns string representing firmware version or NULL or error

        @note
            Memory should not be released by the user.
            When device in sot in HORI_MODE_CONFIG will return NULL.
     */
    const char* HORI_API_CALL hori_get_firmware_version_str(hori_device_t* device);

    /** @brief Return device firmware version

        @param device The device which version need to be read

        @returns
            The function returns firmware version or NULL on error or when device is not in HORI_MODE_CONFIG

        @note
            Memory should not be released by the user.
            When device is not in HORI_MODE_CONFIG will return NULL
     */
    struct hori_firmware_version const* HORI_API_CALL hori_get_firmware_version(hori_device_t* device);

    /** @brief Close HORI device

        @ingroup API
        @since 0.1.0
        @param device The handle returned from @see hori_open
    */
    void HORI_API_CALL hori_close(hori_device_t* device);

    /** @brief Get HORI device state

        @ingroup API
        @since 0.1.0
        @param device The handle returned from @see hori_open

        @returns
            This function return @see hori_state or -1 on error
     */
    int HORI_API_CALL hori_get_state(hori_device_t* device);

    /** @brief get device state

        @ingroup API
        @since 0.1.0
        @param device The handle returned from @see hori_open

        @returns
            This function returns @see hori_state on -1 on error
     */
    int HORI_API_CALL hori_set_state(hori_device_t* device, int state);

    /** @brief Get HORI device config (context based)

        @ingroup API
        @since 0.1.0
        @param device[in] The device which config to retrive

        @returns
            The functions returns pointer to device config or NULL on error
     */
    hori_device_config_t* HORI_API_CALL hori_get_device_config(hori_device_t* device);

    /** @brief Get HORI Vendor Id

        @ingroup API
        @since 0.1.0

        @returns
            This function returns @see HORI_HID_VENDOR_ID
     */
    unsigned short HORI_API_CALL hori_vendor_id();

    /** @brief Get runtime hori api version

        @ingroup API
        @since 0.1.0

        @returns
            Internal structure containing version number

        @note
            This memory should not by released by the user
      */
    const struct hori_software_version* HORI_API_CALL hori_version();

    /** @brief Get runtime hori api version as c-style string

        @ingroup API
        @since 0.1.0

        @returns
            String represetation of version number

        @note
            This memory should not be released by the user
      */
    const char* HORI_API_CALL hori_version_str();

    /** @brief Handle to hori gampad

        @ingroup API
        @since 0.1.0
      */
    typedef struct hori_gamepad hori_gamepad_t;

    hori_gamepad_t* HORI_API_CALL hori_make_gamepad();

    void HORI_API_CALL hori_free_gamepad(hori_gamepad_t* gamepad);

    /** @brief Read current gamepad state

        @ingroup API
        @since 0.1.0
        @param device The device handle @see hori_open
        @param gamepad The gamepad handle @see hori_make_gamepad

        @returns
            This function return -1 on failure, number of bytes read otherwise

        @note
            During HORI_STATE_NORMAL return profile gamepad output report
            During HORI_STATE_CONFIG return read gamepad output report
     */
    int HORI_API_CALL hori_read_gamepad(hori_device_t* device, hori_gamepad_t* gamepad);
    int HORI_API_CALL hori_read_gamepad_timeout(hori_device_t* device, hori_gamepad_t* gamepad, int miliseconds);

    /** @brief Check if button is pressed

        @param gamepad The gamepad handle
        @param button The button id

        @returns
            This function returns -1 if button is not present on gamepad, otherwise
            function returs 1 if button is pressed and 0 if not.
     */
    int HORI_API_CALL hori_get_button(hori_gamepad_t* gamepad, int button);

    /** @brief Get button status

        @summary
            Buttons are returned in set of 31 bits

        @returns
            This function returns -1 on error or when index is not valid, button map otherwise
      */

    int HORI_API_CALL hori_get_buttons(hori_gamepad_t* gamepad, int index);
    /** @brief Get axis value

        @returns
            This function return -1 if axis is not present on gamepad, otherwise
            value between 0..<max-value> for axis
      */
      //
      // value 0 - min
      // value (max-min)/2 - zero
      // value uint16_t - max
    int HORI_API_CALL hori_get_axis(hori_gamepad_t* gamepad, int axis, int prop);
    //https://blog.the.al/2023/01/01/ds4-reverse-engineering.html
    int HORI_API_CALL hori_get_touch_count(hori_gamepad_t* gamepad);
    /** */
    int HORI_API_CALL hori_touch(hori_gamepad_t* gamepad, int touch, int prop);

    enum hori_sensor_property {
        HORI_SENSOR_VALUE = 0,
        HORI_SENSOR_ZERO,
        HORI_SENSOR_NORM, // value for normalization
        HORI_SENSOR_UNIT, // units of value
    };
    enum hori_unit {
        HORI_UNIT_NORMALIZED,
        HORI_UNIT_SIGNED_NORMALIZED,
        HORI_UNIT_ACCEL_VELOCITY,
        HORI_UNIT_GROY_ROTATION_DEEGRESS_PER_SECOND,
    };

    // https://github.com/JibbSmart/JoyShockLibrary/blob/master/JoyShockLibrary/InputHelpers.cpp
    /** @brief Get sensor value
     */
    int HORI_API_CALL hori_sensor(hori_gamepad_t* gamepad, int sensor, int prop);
    /** @brief Handle to hori profile

        @ingroup API
        @since 0.1.0
     */
    typedef struct hori_profile hori_profile_t;

    /** @brief Get new profile handle for product

        @ingroup API
        @since 0.1.0
        @param product The product code

        @returns
            NULL on error or handle
      */
    hori_profile_t* hori_make_profile(int product);

    /** @brief Free profile handle

        @ingroup API
        @since 0.1.0
        @param profile The handle returned from @see hori_make_profile
     */
    void hori_free_profile(hori_profile_t* profile);

    /** @brief Get profile from device or cache

        @ingroup API
        @since 0.1.0
        @param device The device handle
        @param profile The profile number

        @returns
            This function returns NULL on error, or profile handle

        @note
            Returned pointer is to internally allocaded memory and should not be realesead.
            Contents of handle can change due to call @see hori_set_profile.
      */
    hori_profile_t* HORI_API_CALL hori_get_profile(hori_device_t* device, int profile_id);

    /** @brief Set profile to device / cache

        @ingroup API
        @since 0.1.0
        @param device The device handle returned by @see hori_open
        @param profile_id The profile number
        @param profile The profile handle returned by @see hori_make_profile or @see hori_get_profile

        @returns
            The function returns -1 on error or number of bytes that changed in profile

        @note
            This function requires device to be in @see HORI_STATE_CONFIG.
            The @p profile was acquired by @see hori_get_profile it should not be released by the user.
    */
    int hori_set_profile(hori_device_t* device, int profile_id, hori_profile_t* profile);

    /** @brief Get profile name */
    char const* HORI_API_CALL hori_get_profile_name(hori_profile_t* profile);

    int HORI_API_CALL hori_set_profile_name(hori_profile_t* profile, char const* name, int size);
    // Ideas of functions
    //      - int hori_store_profile(hori_device_t* device, int profile_id);
    // same as hori_set_profile(device, profile_id, hori_get_profile(device, profile_id))
    //      - int hori_reset_profile(hori_device_t* device, int profile_id);
    // same as hori_set_profile(device, profile_id, NULL)

    /**
     * @brief Open hori device using product and index (current order of listed devices)
     * @param product[in] product to open
     * @param index[in] index of the product used when multiple devices are found
     * @param context[in] context to open
     */
    hori_device_t* hori_open(hori_product_t product, int index, hori_context_t* context);

    /**
     * @brief open index
     */
    hori_device_t* hori_open_index(int index, hori_context_t* context);

#ifdef __cplusplus
} // extern "C" 
#endif
