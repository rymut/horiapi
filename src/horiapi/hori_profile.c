#include "hori_profile.h"

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
