#include "hori_profile.h"

#include <stdlib.h>

#include <horiapi/horiapi.h>

#include "hori_utf8.h"

int hori_internal_is_valid_button_config(struct hori_button_config* config) {
    if (config == NULL) {
        return -1;
    }
    // not realy sure
    if (config->flag_turbo != config->turbo_enabled) {
        // return -1;
    }
    // also not ok,  
    if (config->flag_mapping != config->map_button.enabled) {
        //        return -1;
    }
    // also not ok? 
    if ((config->enabled != 0) != (config->flags != 0)) {
        //        return -1;
    }
    return 0;
}

int hori_internal_is_valid_profile_config(struct hori_profile_config* config) {
    if (config == NULL) {
        return -1;
    }
    // name validation
    int name_byte_size = hori_internal_utf8_byte_size(config->name, HORI_PROFILE_NAME_SIZE);
    if (name_byte_size == -1) {
        return -1;
    }
    for (++name_byte_size; name_byte_size < HORI_PROFILE_NAME_SIZE; ++name_byte_size) {
        if (config->name[name_byte_size] != 0) {
            return -1;
        }
    }

    for (int button_id = 0; button_id < HORI_PROFILE_BUTTONS_COUNT; ++button_id) {
        if (-1 == hori_internal_is_valid_button_config(config->buttons + button_id)) {
            return -1;
        }
    }
    return 0;
}

hori_profile_t *hori_make_profile(int product) {
    struct hori_profile* result = (struct hori_profile*)calloc(1, sizeof(struct hori_profile));
    if (result == NULL) {
        return NULL;
    }
    result->hori_api_version = HORI_API_VERSION;
    result->product = product;
    result->error_code = HORI_PROFILE_NO_ERROR;
    return result;
}
void hori_free_profile(hori_profile_t* profile) {
    if (profile == NULL) {
        return;
    }
    if (profile->hori_api_version != HORI_API_VERSION) {
        return;
    }
    free(profile);
}

char const* HORI_API_CALL hori_get_profile_name(hori_profile_t* profile) {
    if (profile == NULL) {
        return NULL;
    }
    if (profile->hori_api_version != HORI_API_VERSION) {
        return NULL;
    }
    memset(profile->name, 0, sizeof(profile->name));
    memcpy(profile->name, profile->config.name, sizeof(profile->config.name));
    return profile->name;
}

int HORI_API_CALL hori_set_profile_name(hori_profile_t* profile, char const* name, int size) {
    if (profile == NULL) {
        return -1;
    }
    if (name != NULL && size > HORI_PROFILE_NAME_SIZE) {
        return -1;
    }
    memset(profile->config.name, 0, sizeof(profile->config.name));
    memcpy(profile->config.name, name, size);
    hori_get_profile_name(profile);
    return 0;
}
