#pragma once

#include <inttypes.h>

#include <yaml.h>

#include <horiapi/horiapi.h>

enum hori_yaml_button_naming {
    HID = 0,
    CONFIG = 1, 
    PLAYSTATION = 2,
    XBOX = 3,
    SWITCH = 4
};

/** @brief Store config information
 */
struct hori_yaml_config {
    /** @brief config stored inside profile
     */
    int layout;
    /** @brief Controller type */
    int product;
    /** @brief Profile number
     */
    int profile_id;
    /** @brief Proflie to store */
    hori_profile_t *profile;
};
/** @brief Store information about config
 */
struct hori_yaml_config_list {
    struct hori_yaml_config config;
    struct hori_yaml_config_list* next;
};

struct hori_yaml_config_list* hori_yaml_make_config_list(const hori_device_config_t *device_config, const hori_profile_t *profile);

void hori_yaml_free_config_list(struct hori_yaml_config_list* list);

int hori_yaml_config_parse_file(struct hori_yaml_config_list** config, FILE* file, const hori_context_t *context);
int hori_yaml_config_parse_string(struct hori_yaml_config_list** config, const uint8_t* data, size_t size, const hori_context_t *context);
int hori_yaml_config_list_parse(struct hori_yaml_config_list** config, yaml_parser_t* parser, const hori_context_t *context);

int hori_yaml_config_emit_file(const struct hori_yaml_config_list* config, FILE* file);
int hori_yaml_config_emit_string(const struct hori_yaml_config_list* config, uint8_t* data, size_t size, size_t* writeSize);
int hori_yaml_config_emit(const struct hori_yaml_config_list* config, yaml_emitter_t* emitter);
