#include "hori_device.h"

#include <stdlib.h>

#include "hori_command.h"
#include "hori_profile.h"

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

int hori_internal_device_set_profile(hori_device_t* device, int profile_id, struct hori_profile* value) {
    if (device == NULL) {
        return -1;
    }
    struct hori_device_profile* parent = NULL;
    struct hori_device_profile* profile = NULL;
    for (profile = device->profiles; profile != NULL; parent = profile, profile = profile->next) {
        if (profile->id == profile_id) {
            break;
        }
    }
    // remove profile
    if (value == NULL) {
        if (profile == NULL) {
            return 0;
        }
        if (parent) {
            parent->next = profile->next;
        }
        if (profile == device->profiles) {
            device->profiles = profile->next;
        }
        free(profile);
        return 0;
    }
    if (profile == NULL) {
        profile = (struct hori_device_profile*)calloc(1, sizeof(struct hori_device_profile));
        if (profile == NULL) {
            return -1;
        }
        profile->id = profile_id;
    }
    free(profile->profile);
    profile->profile = value;
    if (parent == NULL) {
        device->profiles = profile;
    }
    else {
        parent->next = profile;
    }
    return 0;
}

struct hori_device_profile* hori_internal_device_get_profile(hori_device_t* device, int profile_id) {
    if (device == NULL) {
        return NULL;
    }
    for (struct hori_device_profile* profile = device->profiles; profile != NULL; profile = profile->next) {
        if (profile->id == profile_id) {
            return profile;
        }
    }
    return NULL;
}

hori_profile_t* HORI_API_CALL hori_get_profile(hori_device_t* device, int profile_id) {
    if (device == NULL) {
        return NULL;
    }
    struct hori_device_profile* device_profile = hori_internal_device_get_profile(device, profile_id);
    if (device_profile != NULL) {
        return device_profile->profile;
    }

    hori_profile_t* profile = hori_make_profile(device->config->hid_config_product_id);
    if (profile == NULL) {
        return NULL;
    }
    if (-1 == hori_internal_read_profile(device, profile_id, &profile->config)) {
        free(profile);
        return NULL;
    }
    if (-1 == hori_internal_device_set_profile(device, profile_id, profile)) {
        free(profile);
        return NULL;
    }
    return profile;
}

// update device->profiles only as last step
int HORI_API_CALL hori_set_profile(hori_device_t* device, int profile_id, hori_profile_t* profile) {
    if (device == NULL) {
        return -1;
    }
    if (profile != NULL && profile->product != device->config->product) {
        return -1;
    }
    // TODO VALIDATE PROFILE
    struct hori_profile_config profile_config;
    memset(&profile_config, 0, sizeof(profile_config));
    if (-1 == hori_internal_read_profile(device, profile_id, &profile_config)) {
        return -1;
    }
    // write profile

    struct hori_device_profile* device_profile = hori_internal_device_get_profile(device, profile_id);
    if (device_profile == NULL) {
        if (profile == NULL) {
            return 0;
        }
        hori_profile_t* dev_profile = hori_make_profile(device->config->product);
        if (dev_profile == NULL) {
            return -1;
        }
        memcpy(&dev_profile->config, &profile_config, sizeof(profile_config));
        if (-1 == hori_internal_device_set_profile(device, profile_id, dev_profile)) {
            free(dev_profile);
            return -1;
        }
        device_profile = hori_internal_device_get_profile(device, profile_id);
    }
    if (device_profile == NULL) {
        // should never happen
        return -1;
    }
    if (profile == NULL) {
        memcpy(&device_profile->profile->config, &profile_config, sizeof(profile_config));
        return 0;
    }
    // update profile data - partialy update
    return hori_internal_write_profile_difference(device, profile_id, &profile->config);
}

