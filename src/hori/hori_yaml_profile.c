#include "hori_yaml_profile.h"

#include "horiapi/hori_context.h"

#include <stdarg.h>

const char config_profile_id[] = "id";
const char config_profile_layout[] = "name";
const char config_profile_device[] = "product";

#define CONFIG_PROFILE_SCALAR_MAX_LENGHT 80

int hori_yaml_parse_profile(yaml_parser_t* parser, struct hori_yaml_config* profile) {
    return 0;
}

int hori_yaml_scaler(char* buffer, int size, const char* format, ...) {
    if (buffer == NULL || size <= 0)
        return 0;
    va_list args;
    memset(buffer, 0, size);
    va_start(args, format);
    int length = vsnprintf(buffer, size - 1, format, args);
    va_end(args);
    if (length < 0 || size - 1 > length)
        return 0;
    return length;
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
    if (!(buffer_size = hori_yaml_scaler(buffer, buffer_size, format, args))) {
        va_end(args);
        return 0;
    }
    va_end(args);
    if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_INT_TAG,
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
    struct hori_profile_config profile_config;
    memset(&profile_config, 0, sizeof(profile_config));
    memcpy(&profile_config, &config->profile->config, sizeof(profile_config));

    yaml_event_t event;

    if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    // profile id
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_id, sizeof(config_profile_id), scalar, sizeof(scalar), "%d", config->id))
        return 0;

    // profile layout
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_layout, sizeof(config_profile_layout), scalar, sizeof(scalar), "%d", config->layout))
        return 0;

    // profile device
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_device, sizeof(config_profile_device), scalar, sizeof(scalar), "%d", config->profile->product))
        return 0;

    // profile name
    if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
        config_profile_device, sizeof(config_profile_device), scalar, sizeof(scalar), "%s", config->profile->name))
        return 0;

    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 0;
}
