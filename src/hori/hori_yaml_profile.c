#include "hori_yaml_profile.h"

#include "horiapi/hori_context.h"

int hori_yaml_parse_profile(yaml_parser_t* parser, struct hori_yaml_config* profile) {
    return 0;
}

int hori_yaml_emit_profile(yaml_emitter_t* emitter, const struct hori_yaml_config* profile) {
    if (emitter == NULL || profile == NULL)
        return 0;

    struct hori_profile_config profile_config;
    memset(&profile_config, 0, sizeof(profile_config));
    yaml_event_t event;

    if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;


    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 0;
}
