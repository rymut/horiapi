#pragma once

#include <horiapi/horiapi.h>

#include <hidapi/hidapi.h>
#include "hori_profile.h"

#if defined(__cplusplus)
extern "C" {
#endif
    /** @brief Platform dependent data */
    typedef struct hori_device_platform_data hori_device_platform_data_t;

    /** @brief Create platform device data

        @param device_config[in]
      */
    hori_device_platform_data_t* hori_make_platform_data(const hori_device_config_t* device_config, const char* path);

    /** @brief Free platform data

        @param platform_data[in] The data to release
      */
    void hori_free_platform_data(hori_device_platform_data_t* platform_data);

    /** @brief Check if platform data is identical

        @param[in] a The first object
        @param[in] b The second object

        @returns
            This function returns 1 if obejct are identical:
                @p a and @p b is the same adress,
                @p a and @p b is platfrom data extracted from the same phisical usb port and points to the same device (platform dependent implementation)
      */
    int hori_platform_data_is_identical(const hori_device_platform_data_t* a, const hori_device_platform_data_t* b);

    struct hori_device_profile {
        int id;
        struct hori_profile* profile;
        struct hori_device_profile* next;
    };

    /** @brief Device handle */
    struct hori_device {
        /** @brief Version of hori api */
        int hori_api_version;
        /** @brief Context on with device operates */
        hori_context_t* context;
        /** @brief Device config */
        hori_device_config_t* config;
        /** @brief HID device for gampad access */
        hid_device* gamepad;
        /** @brief HID device for control access */
        hid_device* control;
        /** @brief Platform dependent data */
        hori_device_platform_data_t* platform_data;
        /** @brief Firmware version read from device

            @note
                Released each time when MODE changes from/to CONFIG
         */
        char* firmware_version_str;
        /** @brief Firmware version information */
        struct hori_firmware_version* firmware_version;

        /** @brief Profile data read from device */
        struct hori_device_profile* profiles;
    };

    /** @brief Write data to control interface

        @param device[in|out] The device to write to
        @param data[in] The memory that will be send to device
        @param size[in] The size of array passed in @param data

        @returns
            The function returns -1 on error, 0 otherwise
      */
    int hori_device_write_control(hori_device_t* device, uint8_t* data, int size);

    /** @brief Read data from control interface
     */
    int hori_device_read_control(hori_device_t* device, uint8_t* data, int size);

    int hori_device_read_control_timeout(hori_device_t* device, uint8_t* data, int size, int miliseconds);

    int hori_device_read_control_retry(hori_device_t* device, uint8_t* data, int size);

    /** @brief Set profile for device

        @param device[in|out] The device handle
        @param profile_id[in] Profile number
        @param config[in] The config - on null profile is removed from device

        @returns
            This function returns -1 on error and 0 on success
      */
    int hori_device_set_profile(hori_device_t* device, int profile_id, struct hori_profile* config);

    /** @brief Get profile from device

        @param device[in] The device handle
        @param profile_id[in] The profile id

        @returns
            This function returns profile handle if profile exists or nullptr otherwise.
            Profile pointer is buffer menaged by device so accessing after device is disconnected is invalid
      */
    struct hori_device_profile* hori_device_get_profile(hori_device_t* device, int profile_id);
#if defined(__cplusplus)
}
#endif 
