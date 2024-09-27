#pragma once

#include <horiapi/horiapi.h>

#include <hidapi/hidapi.h>

#if defined(__cplusplus)
extern "C" {
#endif
    typedef struct hori_device_platform_data hori_device_platform_data_t;

    struct hori_device {
        hori_context_t* context;
        hid_device* gamepad;
        hid_device* control;
        hori_device_platform_data_t* platform_data;
        /**
         * @brief Firmware version read from device
         *
         * Released each time when MODE changes from/to CONFIG
         */
        char* firmware_version_str;
        struct hori_firmware_version* firmware_version;
        /**
         * @brief profile data
         */
        char* profiles;
    };

    /**
     @brief Write data to control interface
     */
    int hori_internal_write_control(hori_device_t* device, uint8_t* data, int size);
    /**
     @brief Read data from control interface
     */
    int hori_internal_read_control(hori_device_t* device, uint8_t* data, int size);

#if defined(__cplusplus)
}
#endif 
