#include "hori_yaml_profile.h"

#include "hori_input_names.h"

#include <stdarg.h>

const char config_profile_id[] = "id";
const char config_profile_layout[] = "layout";
const char config_profile_device[] = "product";
const char config_profile_name[] = "name";
const char config_profile_buttons[] = "buttons";

#define CONFIG_PROFILE_SCALAR_MAX_LENGHT 80

int hori_yaml_parse_profile(yaml_parser_t* parser, struct hori_yaml_config* profile) {
    return 0;
}

int hori_yaml_scaler_args(char* buffer, int size, const char* format, va_list args) {
    if (buffer == NULL || size <= 0)
        return 0;
    memset(buffer, 0, size);
    int length = vsnprintf(buffer, size - 1, format, args);
    if (length < 0 || length > size - 1)
        return 0;
    return length;
}
int hori_yaml_scaler(char* buffer, int size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = hori_yaml_scaler_args(buffer, size, format, args);
    va_end(args);
    return result;
}

int hori_yaml_emit_map_scalar(yaml_emitter_t* emitter, const yaml_char_t* anchor, const yaml_char_t* tag, int plain_implicit, int quoted_implicit,
    yaml_scalar_style_t style, const char* name, int name_size, char* buffer, int buffer_size, const char* format, ...) {
    if (emitter == NULL)
        return 0;
    if (name == NULL || name_size < 0)
        return 0;
    if (buffer == NULL || buffer_size < 0 || format == NULL)
        return 0;
    yaml_event_t event;
    if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_STR_TAG,
        name, name_size, 1, 0, YAML_PLAIN_SCALAR_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    va_list args;
    va_start(args, format);
    if (!(buffer_size = hori_yaml_scaler_args(buffer, buffer_size, format, args))) {
        va_end(args);
        return 0;
    }
    va_end(args);
    if (!yaml_scalar_event_initialize(&event, NULL, tag,
        buffer, buffer_size, 1, 0, YAML_PLAIN_SCALAR_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 1;
}
int hori_yaml_emit_profile(yaml_emitter_t* emitter, const struct hori_yaml_config* config) {
    if (emitter == NULL || config == NULL || config->profile == NULL)
        return 0;


    char scalar[CONFIG_PROFILE_SCALAR_MAX_LENGHT] = "";
    int scalar_length = 0;

    yaml_event_t event;

    // profile (map)
    if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    // profile id
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_id, strlen(config_profile_id), scalar, sizeof(scalar), "%d", config->profile_id))
        return 0;

    // profile layout
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_layout, strlen(config_profile_layout), scalar, sizeof(scalar), "%d", config->layout))
        return 0;

    
    // profile device
    // @todo profile->product is private implementation
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_device, strlen(config_profile_device), scalar, sizeof(scalar), "%d", hori_get_profile_product(config->profile)))
        return 0;

    // profile name
    char profile_name_default[80] = "default";
    const char *profile_name = hori_get_profile_name(config->profile);
    if (strlen(profile_name) == 0)
        profile_name = profile_name_default;
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_name, strlen(config_profile_name), scalar, sizeof(scalar), "%s", profile_name))
        return 0;

    // profile buttons (map)
    if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_STR_TAG,
        config_profile_buttons, strlen(config_profile_buttons), 1, 0, YAML_PLAIN_SCALAR_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    for (int index = 0, button = -1; (button = hori_get_profile_button(config->profile, index, HORI_PROFILE_BUTTON_NAME)) != -1; ++index) {
        int any_enabled = hori_get_profile_button(config->profile, index, HORI_PROFILE_BUTTON_ANY_ENABLED);
        if (any_enabled != 1)
            continue;
        const char *button_name = hori_get_button_name(button, 4);
        if (button_name == NULL)
            return 0;
        // button content
        if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_STR_TAG,
            button_name, strlen(button_name), 1, 0, YAML_PLAIN_SCALAR_STYLE))
            return 0;
        if (!yaml_emitter_emit(emitter, &event))
            return 0;
        if (!hori_yaml_emit_profile_button(emitter, config->profile, index))
            return 0;
    }
    // profile (button end)
    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    // profile (map end)
    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 1;
}

int hori_yaml_emit_profile_button(yaml_emitter_t* emitter, const hori_profile_t* profile, int button) {
    if (emitter == NULL || profile == NULL)
        return 0;
    int value = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANY_ENABLED);
    if (value == -1)
        return 0;
    yaml_event_t event;
    if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    if (value) {
        // serialize fields
    }
    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 1;
}
