#include <horiapi/horiapi.h>

#include <time.h>

#include "hori_time.h"
#include "hori_device.h"
#include "hori_context.h"

int HORI_API_CALL hori_get_state(hori_device_t* device) {
    if (device == NULL) {
        return HORI_STATE_NONE;
    }
    if (device->gamepad == NULL) {
        return HORI_STATE_NONE;
    }
    struct hid_device_info* dev_info = hid_get_device_info(device->gamepad);
    hori_device_config_t* device_config = hori_internal_device_config_find(device->context->devices, dev_info);
    if (device_config == NULL) {
        return HORI_STATE_NONE;
    }
    if (device_config->hid_normal_product_id == dev_info->product_id) {
        return HORI_STATE_NORMAL;
    }
    if (device_config->hid_config_product_id == dev_info->product_id && device->control != NULL) {
        return HORI_STATE_CONFIG;
    }
    return HORI_STATE_NONE;
}

int hori_internal_is_xinput(hori_device_t* device);
int hori_internal_send_enter_config(hori_device_t* device);
// platform dependent (win32)
int hori_internal_send_enter_config_xinput(hori_device_t* device);
int set_rumble_sequence(const wchar_t* path);
int set_rumble(HANDLE handle, uint16_t low, uint16_t high);

int set_rumble(HANDLE handle, uint16_t low, uint16_t high)
{
    if (handle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    //BYTE rumble_packet[] = { 0x00, 0x08, low >> 8, high >> 8, 0x00, 0x00, 0x00, 0x00 };
    BYTE in[5] = { 0, 0, low >> 8 , high >> 8, 2 };
    if (!DeviceIoControl(handle, 0x8000a010, in, sizeof(in), NULL, 0, NULL, NULL))
        //if (!DeviceIoControl(handle, 0x8000a010, rumble_packet, sizeof(rumble_packet), NULL, 0, NULL, NULL))
    {
        // NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
        return -1;
    }
    return 0;
}

int set_rumble_sequence(const wchar_t* path) {
    DWORD delay = 5, time = 15;
    DWORD sleep[] = { delay, time, delay, time, delay, time, delay, time, delay };
    WORD values[] = { 0, 13311, 0, 6655, 0, 13311, 0, 32767, 0 };
    const size_t steps = sizeof(sleep) / sizeof(DWORD);
    HANDLE handle = OpenDeviceInterface(path, FALSE);
    if (handle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    for (size_t i = 0; i < steps; ++i) {
        if (-1 == set_rumble(handle, values[i], values[i])) {
            CloseHandle(handle);
            return -1;
        }
        Sleep(sleep[i]);
    }
    CloseHandle(handle);
    return 0;
}



#include "hori_string_win32.h"
#include "hori_device_win32.h"

int hori_internal_send_enter_config_xinput(hori_device_t* device) {
    GUID xboxClassGuid = { 0xec87f1e3, 0xc13b, 0x4100, { 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb } };
    wchar_t* list = NULL;
    ULONG len = 0;
    wchar_t* device_id = hori_win32_device_get_physical_device_intance_id(hid_get_device_info(device->gamepad)->path);
    if (device_id == NULL) {
        return -1;
    }

    struct hori_wstring_list device_interfaces = hori_win32_device_get_device_interfaces_wstring_list(device_id, &xboxClassGuid);
    free(device_id);

    if (device_interfaces.count < 1) {
        hori_clear_wstring_list(&device_interfaces);
        return -1;
    }
    //query_controller(list);
    set_rumble_sequence(device_interfaces.value);
    hori_clear_wstring_list(&device_interfaces);
    return 0;
}

int hori_internal_is_xinput(hori_device_t* device) {
    if (device == NULL) {
        return 0;
    }
    return device->control == NULL;
}

int hori_internal_send_enter_config(hori_device_t* device) {
    if (hori_internal_is_xinput(device)) {
        return hori_internal_send_enter_config_xinput(device);
    }
    else {
        return hori_internal_send_enter_config_hid(device);
    }
}

int HORI_API_CALL hori_set_state(hori_device_t* device, int state) {
    if (device == NULL) {
        return HORI_STATE_NONE;
    }
    int current = hori_get_state(device);
    if (current == state) {
        return state;
    }
    hori_device_config_t* device_config = hori_get_device_config(device);
    unsigned short product_id = 0;
    unsigned short usage_page_gamepad = 0;
    unsigned short usage_page_control = 0;
    if (device_config == NULL) {
        return HORI_STATE_NONE;
    }
    if (current == HORI_STATE_CONFIG) {
        hori_internal_leave_config(device);
        product_id = device_config->hid_normal_product_id;
        usage_page_gamepad = device_config->hid_normal_usage_page_gamepad;
        usage_page_control = device_config->hid_normal_usage_page_control;
    }
    if (current == HORI_STATE_NORMAL) {
        if (-1 == hori_internal_send_enter_config(device)) {
            return current;
        }
        product_id = device_config->hid_config_product_id;
        usage_page_gamepad = device_config->hid_config_usage_page_gamepad;
        usage_page_control = device_config->hid_config_usage_page_profile;
    }
    hid_close(device->gamepad);
    device->gamepad = NULL;
    hid_close(device->control);
    device->control = NULL;


    for (hori_clock_t prev = hori_clock_now(), now = prev;
        hori_clock_diff(prev, now) * 1000 < device->context->rediscover_miliseconds_timeout;
        hori_sleep_ms(device->context->rediscover_miliseconds_delay), now = hori_clock_now()) {
        if (-1 != hori_internal_open(device, product_id, usage_page_gamepad, usage_page_control)) {
            if (-1 == hori_send_heartbeat(device)) {
                hid_close(device->gamepad);
                device->gamepad = NULL;
                hid_close(device->control);
                device->control = NULL;
                return HORI_STATE_NONE;
            }
            return state;
        }
    }
    return HORI_STATE_NONE;
}

