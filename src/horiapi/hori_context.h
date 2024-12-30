#pragma once

#include <horiapi/horiapi.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** @brief Get defualt context (readonly)

        @returns
            THis function returns default cotntext
      */
    const hori_context_t* hori_internal_context();
    hori_device_config_t* hori_internal_device_config_find(hori_device_config_t* list_front, struct hid_device_info* info);
#ifdef __cplusplus
} // extern "C" 
#endif
