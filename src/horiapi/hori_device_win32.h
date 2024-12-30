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
#if defined(__cplusplus)
}
#endif 
