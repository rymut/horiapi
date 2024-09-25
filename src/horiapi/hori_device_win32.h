#include "hori_device.h"
#include <wchar.h>

#if defined(__cplusplus)
extern "C" {
#endif
    struct hori_internal_string_list {
        wchar_t *value;
        int count;
        int byte_size;
    };
    /**
     * Platform data
     */
    struct hori_device_platform_data {
        struct hori_internal_string_list location;
    };

    typedef struct hori_device_platform_data hori_device_platform_data_t;

    hori_device_platform_data_t* hori_internal_platform_data();
    void hori_internal_free_platform_data(hori_device_platform_data_t* data);
#if defined(__cplusplus)
}
#endif 
