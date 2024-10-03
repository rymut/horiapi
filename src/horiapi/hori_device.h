#pragma once

#include <horiapi/horiapi.h>

#include <hidapi/hidapi.h>
#include "hori_profile.h"

#if defined(__cplusplus)
extern "C" {
#endif
    typedef struct hori_device_platform_data hori_device_platform_data_t;

    struct hori_device_profile {
        int id;
        struct hori_profile *profile;
        struct hori_device_profile* next;
    };

    /** @brief Device handle */
    struct hori_device {
        /** @brief Version of hori api */
        int hori_api_version;
        hori_context_t* context;
        /** @brief Device config */
        hori_device_config_t* config;
        hid_device* gamepad;
        hid_device* control;
        hori_device_platform_data_t* platform_data;
        /** @brief Firmware version read from device

            @note
                Released each time when MODE changes from/to CONFIG
         */
        char* firmware_version_str;
        struct hori_firmware_version* firmware_version;

        /** @brief profile data */
        struct hori_device_profile* profiles;
    };

    /** @brief Write data to control interface
     */
    int hori_internal_write_control(hori_device_t* device, uint8_t* data, int size);
    /** @brief Read data from control interface
     */
    int hori_internal_read_control(hori_device_t* device, uint8_t* data, int size);

    int hori_internal_read_control_timeout(hori_device_t* device, uint8_t* data, int size, int miliseconds);

    int hori_internal_read_control_retry(hori_device_t* device, uint8_t* data, int size);

    /** @brief Set profile for device

        @param device[in|out] The device handle
        @param profile Profile number
        @param config[in] The config - on null profile is removed from device

        @returns
            This function returns -1 on error and 0 on success
      */
    int hori_internal_device_set_profile(hori_device_t* device, int profile_id, struct hori_profile* config);
    struct hori_device_profile* hori_internal_device_get_profile(hori_device_t* device, int profile_id);
#if defined(__cplusplus)
}
#endif 
