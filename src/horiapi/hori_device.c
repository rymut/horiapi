#include "hori_device.h"

int hori_internal_write_control(hori_device_t* device, uint8_t* data, int size) {
    if (device == NULL) {
        return -1;
    }
    if (device->control == NULL) {
        return -1;
    }
    if (data == NULL || size <= 0) {
        return -1;
    }
    return hid_write(device->control, data, size);
}

int hori_internal_read_control(hori_device_t* device, uint8_t* data, int size) {
    if (device == NULL) {
        return -1;
    }
    if (device->control == NULL) {
        return -1;
    }
    if (data == NULL || size <= 0) {
        return -1;
    }
    memset(data, 0, size);
    return hid_read_timeout(device->control, data, size, device->context->read_timeout_ms);
}

void HORI_API_CALL hori_close(hori_device_t* device) {
    if (device) {
        hori_internal_free_platform_data(device->platform_data);
        hid_close(device->control);
        hid_close(device->gamepad);
        free(device->firmware_version);
        free(device->profiles);
        free(device);
    }
}
