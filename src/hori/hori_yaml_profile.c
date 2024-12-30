#include "hori_yaml_profile.h"

#include "horiapi/hori_context.h"

int hori_yaml_parse_profile(yaml_parser_t* parser, struct hori_profile* profile, int* platform) {
    return 0;
}

int hori_yaml_emit_profile(yaml_emitter_t* emitter, const struct hori_profile* profile, int platform) {
    if (emitter == NULL || profile == NULL)
        return 0;

    int product = HORI_PRODUCT_ANY;
    struct hori_profile_config profile_config;
    memset(&profile_config, 0, sizeof(profile_config));

    
     
    return 0;
}
