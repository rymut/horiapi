#include "hori_device.h"
#include <wchar.h>
#include <Windows.h>
#include <Devpropdef.h>
#include <Cfgmgr32.h>
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
    struct hori_wstring_list hori_win32_device_get_device_interfaces_wstring_list(const wchar_t* device_id, LPCGUID interface_class_guid);

    /** @brief Get device instence id

        @param[in] path

        @returns
            The function returns device instance id or NULL if path is not pointing to device
     */
    wchar_t* hori_win32_device_get_physical_device_intance_id(const char* path);

    /** @brief Get information about physical device

        @param path[in] The device or device endpoint to get physical device info
        @param device_instance_id[out] Pointer to empty pointer where device instance id will be stored
        @param location_paths[out] Pointer to empty string list where data locations will be stored
    */
    int hori_win32_device_get_physical_device(const char* path, wchar_t** device_instance_id, struct hori_wstring_list* location_paths);

    /** @brief Check if device instance handle is controller (HID or XINPUT)

        @param[in] device_instance The instance to HID device

        @returns
            The function returns -1 on error, 0 if device is not a controller device class, else 1
     */
    int hori_win32_device_instance_is_controller(DEVINST device_instance);

    /** @brief Get device instance guid property value

    @param[in]  device_instance  The device instance
    @param[in]  property_key     The property key
    @param[out] property_value   The memory where to store property value

    @returns
        This function returns -1 on error, 0 on success property of type GUID exists on device instance
 */
    int hori_win32_device_instance_get_guid_property(DEVINST device_instance, const PDEVPROPKEY property_key, GUID* property_value);

#if defined(__cplusplus)
    }
#endif 
