#pragma once

#include <horiapi/horiapi.h>

#include <hidapi/hidapi.h>

#if defined(__cplusplus)
extern "C" {
#endif
    typedef struct hori_device_platform_data hori_device_platform_data_t;

    struct hori_device {
        hori_context_t* context;
        struct hid_device* gamepad;
        struct hid_device* control;
        hori_device_platform_data_t* platform_data;
        /**
         * @brief Firmware version read from device
         */
        char* firmware_version;
        /**
         * @brief profile data
         */
        char* profiles;
    };
#if defined(__cplusplus)
    }
#endif 
