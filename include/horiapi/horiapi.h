#pragma once

#include <inttypes.h>

#define HORI_API_CALL 

#define HORI_API_VERSION_MAJOR 0
#define HORI_API_VERSION_MINOR 1
#define HORI_API_VERSION_PATCH 0

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
#define HORI_API_AS_STR_IMPL(x) #x
#define HORI_API_AS_STR(x) HORI_API_AS_STR_IMPL(x)
#define HORI_API_TO_VERSION_STR(v1, v2, v3) HORI_API_AS_STR(v1.v2.v3)

/** @brief Compile-time c-string version of the library.

    @summary Stored as "<major>.<minor>.<patch>"

    @ingroup API
*/
#define HORI_API_VERSION_STR HORI_API_TO_VERSION_STR(HORI_API_VERSION_MAJOR, HORI_API_VERSION_MINOR, HORI_API_VERSION_PATCH)

#ifdef __cplusplus
extern "C" {
#endif
    enum hori_product {
        HORI_PRODUCT_ANY = 0, // hori device but might be unsupported
        HORI_PRODUCT_SPF_023,
    };
    typedef enum hori_product hori_product_t;

    enum hori_state {
        HORI_STATE_NONE = -1,
        HORI_STATE_CONFIG = 1,
        HORI_STATE_NORMAL = 2
    };

    /** @brief Controller support */
    enum hori_controller {
        HORI_CONTROLLER_HID = 0,			// configuration option			
        HORI_CONTROLLER_XINPUT,		// windows mode
        HORI_CONTROLLER_PLAYSTATION4,	// playstation 4 mode
        HORI_CONTROLLER_PLAYSTATION5,	// playstation 5 mode
    };

    struct hori_software_version {
        int major;
        int minor;
        int patch;
    };

    struct hori_firmware_version {
        // hardware version
        int hardware_revision;
        struct hori_software_version software_version;
    };

    struct hori_device_firmware_config {
        void* data;
    };
    typedef struct hori_device_firmware_config hori_device_firmware_config_t;

    /**
     * @brief Device config - used to describe supported devices
     */
    struct hori_device_config {
        // must match firmware name
        char firmware_name[16];
        hori_product_t product : 8; // 4
        // normal mode hid vid/pid
        enum hori_controller device_config_mode : 8; //4

        // unsigned short hid_vendor_id; // always 0xF04; ?
        unsigned short hid_normal_product_id;
        /**
         * @brief Product ID for config mode, 0 if unsupported
         */
        unsigned short hid_config_product_id;
        /**
         * @brief Gamepad usage page
         */
        unsigned short hid_normal_usage_page_gamepad; // gamepad usage page
        unsigned short hid_normal_usage_page_control;   // 0 - entry not supporeted (or Xinput)
        unsigned short hid_config_usage_page_gamepad;
        unsigned short hid_config_usage_page_profile;

        struct hori_device_config* next;
    };
    typedef struct hori_device_config hori_device_config_t;

    /** @brief Context settings */
    struct hori_context {
        /** @brief supported device list or null */
        hori_device_config_t* devices;
        /** @brief timeout between rediscover */
        int rediscover_miliseconds_timeout;
        int rediscover_miliseconds_delay;
        /** @brief read timeout */
        int read_timeout_ms;
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
        hori_device_config_t* device_config;
        struct hori_enumeration* next;
    };
    typedef struct hori_enumeration hori_enumeration_t;

    typedef struct hori_buttons hori_buttons_t;
    typedef struct hori_profile hori_profile_t;
    typedef struct hori_gamepad hori_gamepad_t;



    // set custom device config
    // get defalut context
    hori_context_t* hori_context();

    hori_enumeration_t* hori_enumerate(hori_product_t product, hori_context_t* context);
    void hori_free_enumerate(hori_enumeration_t* e);

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

    /**
     * @brief Open path (using any rest is constructed
     */
    hori_device_t* hori_open_path(char* any_path, hori_context_t* context);

    /**
     * @brief Open device using info structure
     */
    hori_device_t* hori_open_info(hori_device_info_t* info, hori_context_t* context);

    /**
     * @brief Write heartbeat to device
     * @param[in] device - device to write heartbeat
     */
    int hori_send_heartbeat(hori_device_t* device);

    /** @brief Get firmware version

        @since 0.1.0

        @returns
            This function returns string representing firmware version or NULL or error

     */
    const char* HORI_API_CALL hori_get_firmware_version_str(hori_device_t *device);


    const struct hori_firmware_version* HORI_API_CALL hori_get_firmware_version(hori_device_t *device);

    /** @brief return firmware version in format
     * @param version - at max 60 characters long
     * @param version_size buffer size
     * int return length of version
     */
    int hori_read_firmware_version(hori_device_t* device, char* version, int version_size);

    /** @brief Close HORI device

        @ingroup API
        @since 0.1.0
        @param device The handle returned from @see hori_open
    */
    void HORI_API_CALL hori_close(hori_device_t* device);

    /** @brief Get HORI device state

        @since 0.1.0
        @param device The handle returned from @see hori_open

        @returns
            This function return @see hori_state or -1 on error
     */
    int HORI_API_CALL hori_get_state(hori_device_t* device);

    /**
     * @brief get device state
     */
    int hori_set_state(hori_device_t* device, int state);

    hori_device_config_t* hori_get_device_config(hori_device_t* device);

    /** @brief Get current device mode

        @ingroup API
        @since 0.1.0
        @param device A handle returned from @see hori_open

        @returns
            This function returns device mode @see hori_mode and -1 on error
     */
    int hori_get_mode(hori_device_t* device);

    int hori_profile_enter(hori_device_t*, int profile);
    int hori_profile_exit(hori_device_t*);
    int hori_profile_switch(hori_device_t*, int profile);
    int hori_profile_get(hori_device_t*, hori_profile_t*);
    int hori_profile_set(hori_device_t*, hori_profile_t*);
    int hori_gamepad_get(hori_device_t*, hori_gamepad_t*);

    /** @brief Get HORI vendor id

        @returns
            This function returns @see HORI_HID_VENDOR_ID
     */
    unsigned short HORI_API_CALL hori_vendor_id();


    /** @brief Get runtime hori api version

        @since 0.1.0

        @returns
            Internal structure containing version number

        @note
            This memory should not by released by the user
      */
    const struct hori_software_version* HORI_API_CALL hori_version();

    /** @brief Get runtime hori api version as c-style string

        @since 0.1.0

        @returns
            String represetation of version number

        @note
            This memory should not be released by the user
      */
    const char* HORI_API_CALL hori_version_str();

#ifdef __cplusplus
} // extern "C" 
#endif
