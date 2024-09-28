#include "hori_device.h"
#include "hori_command.h"

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

unsigned short HORI_API_CALL hori_vendor_id() {
    return HORI_HID_VENDOR_ID;
}

const char* HORI_API_CALL hori_get_firmware_version_str(hori_device_t* device) {
    if (device == NULL) {
        return NULL;
    }
    int state = hori_get_state(device);
    if (state != HORI_STATE_CONFIG) {
        return NULL;
    }
    if (device->firmware_version_str != NULL && device->firmware_version_str[0] != 0) {
        return device->firmware_version_str;
    }
    if (-1 == hori_internal_read_firmware_version(device)) {
        return NULL;
    }
    return device->firmware_version_str;
}

const struct hori_firmware_version* HORI_API_CALL hori_get_firmware_version(hori_device_t* device) {
    if (device == NULL) {
        return NULL;
    }
    int state = hori_get_state(device);
    if (state != HORI_STATE_CONFIG) {
        return NULL;
    }
    if (device->firmware_version != NULL && device->firmware_version->hardware_revision >= 0) {
        return device->firmware_version;
    }
    if (-1 == hori_internal_read_firmware_version(device)) {
        return NULL;
    }
    return device->firmware_version;
}
