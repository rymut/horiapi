#include "hori_device.h"
#include <wchar.h>
#include <Windows.h>

#include "hori_string_win32.h"

#if defined(__cplusplus)
extern "C" {
#endif
    /**
     * Platform data
     */
    struct hori_device_platform_data {
        struct hori_wstring_list location;
    };

    /** @brief Get device interfaces

        @param device_id[in] The device id
        @param interface_class_guid[in] The GUID to enumerate interface

        @returns
            The function returns string list structure, user is responsible for allocated resource
      */
    struct hori_wstring_list hori_device_win32_get_device_interfaces_wstring_list(const wchar_t* device_id, LPCGUID interface_class_guid);

    wchar_t *hori_device_win32_get_physical_device_intance_id(const char* path);
#if defined(__cplusplus)
}
#endif 
