#include "hori_profile.h"

#include <stdlib.h>

#include <horiapi/horiapi.h>

#include "hori_utf.h"

union hori_device_index_input index_input[] = {
    { 0, HORI_BUTTON_UP },
    { 1, HORI_BUTTON_DOWN },
    { 2, HORI_BUTTON_LEFT },
    { 3, HORI_BUTTON_RIGHT },

    { 4, HORI_BUTTON_L1, },
    { 5, HORI_BUTTON_R1, },
    { 6, HORI_BUTTON_L3, },
    { 7, HORI_BUTTON_R3, },

    { 8, HORI_BUTTON_FL1, },
    { 9, HORI_BUTTON_FL2 },
    { 10, HORI_BUTTON_FR1 },
    { 11, HORI_BUTTON_FR2 },

    { 12, HORI_BUTTON_L2 },
    { 13, HORI_BUTTON_R2 },

    { 14, HORI_BUTTON_CROSS },
    { 15, HORI_BUTTON_CIRCLE },
    { 16, HORI_BUTTON_SQUARE },
    { 17, HORI_BUTTON_TRIANGLE },

    { 18, HORI_BUTTON_LPEDAL },
    { 19, HORI_BUTTON_RPEDAL },

    { 20, HORI_BUTTON_PLAYSTATION },
    { 21, HORI_BUTTON_SELECT },
    { 22, HORI_BUTTON_START },
    { 23, HORI_BUTTON_CAMERA },
};

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

hori_profile_t* hori_make_profile(int product) {
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

hori_profile_t* hori_duplicate_profile(const hori_profile_t* profile) {
    if (profile == NULL)
        return NULL;
    if (profile->hori_api_version != HORI_API_VERSION)
        return NULL;
    struct hori_profile* result = (struct hori_profile*)calloc(1, sizeof(struct hori_profile));
    if (result == NULL)
        return NULL;
    memcpy(result, profile, sizeof(struct hori_profile));
    return result;
}

char const* HORI_API_CALL hori_get_profile_name(const hori_profile_t* profile) {
    if (profile == NULL) {
        return NULL;
    }
    if (profile->hori_api_version != HORI_API_VERSION) {
        return NULL;
    }
    return profile->name;
}

int HORI_API_CALL hori_set_profile_name(hori_profile_t* profile, char const* name, int size) {
    if (profile == NULL) {
        return -1;
    }
    if (name == NULL || size < 0 || size > HORI_PROFILE_NAME_SIZE)
        return -1;

    memset(profile->config.name, 0, sizeof(profile->config.name));
    memcpy(&profile->config.name, name, size);
    memset(profile->name, 0, sizeof(profile->name));
    return 0;
}

int HORI_API_CALL hori_set_profile_button(hori_profile_t* profile, int button, int prop, int value) {
    if (profile == NULL)
        return -1;
    if (button < 0 || button >= HORI_PROFILE_BUTTONS_COUNT)
        return -1;
    if (prop < HORI_PROFILE_BUTTON_NAME || prop > HORI_PROFILE_BUTTON_ANALOG_RESPONSE_B_VALUE)
        return -1;
    struct hori_button_config* button_config = profile->config.buttons + button;
    switch (prop) {
    case HORI_PROFILE_BUTTON_NAME:
    case HORI_PROFILE_BUTTON_ANY_ENABLED:
        return -1;
    case HORI_PROFILE_BUTTON_MAPPING_ENABLED:
        if (value < 0 || value > 1)
            return -1;
        button_config->flag_mapping = value;
        button_config->map_button.enabled = value;
        return 0;
    case HORI_PROFILE_BUTTON_MAPPING_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->map_button.value = value;
        return 0;
    case HORI_PROFILE_BUTTON_DEAD_RANGE_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->dead_range = value;
        return 0;
    case HORI_PROFILE_BUTTON_EDGE_DEAD_RANGE_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->edge_dead_range = value;
        return 0;
    case HORI_PROFILE_BUTTON_TURBO_ENABLED:
        if (value < 0 || value > 1)
            return -1;
        button_config->flag_turbo = value;
        button_config->quick_turbo_enabled = value;
        button_config->turbo_enabled = value;
        return 0;
    case HORI_PROFILE_BUTTON_TURBO_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->turbo_speed = value;
        return 0;
    case HORI_PROFILE_BUTTON_ANALOG_LINEAR_ENABLED:
        if (value < 0 || value > 255)
            return -1;
        return button_config->linear_analog_enabled;
    case HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_A_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->map_analog[0].movement = value;
        return 0;
    case HORI_PROFILE_BUTTON_ANALOG_RESPONSE_A_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->map_analog[0].response = value;
        return 0;
    case HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_B_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->map_analog[1].movement = value;
        return 0;
    case HORI_PROFILE_BUTTON_ANALOG_RESPONSE_B_VALUE:
        if (value < 0 || value > 255)
            return -1;
        button_config->map_analog[1].response = value;
        return 0;
    }
    return -1;
}

int HORI_API_CALL hori_get_profile_button(const hori_profile_t* profile, int button, int prop) {
    if (profile == NULL)
        return -1;
    if (button < 0 || button >= HORI_PROFILE_BUTTONS_COUNT)
        return -1;
    if (prop < HORI_PROFILE_BUTTON_NAME || prop > HORI_PROFILE_BUTTON_TURBO_VALUE)
        return -1;
    struct hori_button_config* button_config = profile->config.buttons + button;
    switch (prop) {
    case HORI_PROFILE_BUTTON_NAME:
        return button + 1;
    case HORI_PROFILE_BUTTON_ANY_ENABLED:
        if (button_config->flags
            || button_config->enabled
            || button_config->quick_turbo_enabled
            || button_config->map_button.enabled)
            return 1;
        return 0;
    case HORI_PROFILE_BUTTON_MAPPING_ENABLED:
        return button_config->flag_mapping != 0 && button_config->map_button.enabled;
    case HORI_PROFILE_BUTTON_MAPPING_VALUE:
        return button_config->map_button.value;
    case HORI_PROFILE_BUTTON_DEAD_RANGE_VALUE:
        return button_config->dead_range;
    case HORI_PROFILE_BUTTON_EDGE_DEAD_RANGE_VALUE:
        return button_config->edge_dead_range;
    case HORI_PROFILE_BUTTON_TURBO_ENABLED:
        if (button_config->flag_turbo | button_config->quick_turbo_enabled)
            return 1;
        return 0;
    case HORI_PROFILE_BUTTON_TURBO_VALUE:
        return button_config->turbo_speed;
    case HORI_PROFILE_BUTTON_ANALOG_LINEAR_ENABLED:
        return button_config->linear_analog_enabled;
    case HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_A_VALUE:
        return button_config->map_analog[0].movement;
    case HORI_PROFILE_BUTTON_ANALOG_RESPONSE_A_VALUE:
        return button_config->map_analog[0].response;
    case HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_B_VALUE:
        return button_config->map_analog[1].movement;
    case HORI_PROFILE_BUTTON_ANALOG_RESPONSE_B_VALUE:
        return button_config->map_analog[1].response;
    }
    return -1;
}
