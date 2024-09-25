#pragma once

#include <horiapi/horiapi.h>

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @brief Get defualt context (readoonly)
     */
    const hori_context_t* hori_internal_context();
    hori_device_config_t* hori_internal_find_device_config(hori_device_config_t* list_front, struct hid_device_info* info);
#ifdef __cplusplus
} // extern "C" 
#endif
