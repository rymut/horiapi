#pragma once

#include <inttypes.h>

#include <yaml.h>

#include <horiapi/hori_profile.h>

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
    /** @button config stored inside profile
     */
    int layout;
    /** @brief Profile number
     */
    int id;
    /** @brief Proflie to store */
    struct hori_profile* profile;
};
/** @brief Store information about config
 */
struct hori_yaml_config_list {
    struct hori_yaml_config config;
    struct hori_yaml_config_list* next;
};

int hori_yaml_config_parse_file(struct hori_yaml_config_list* config, FILE* file);
int hori_yaml_config_parse_string(struct hori_yaml_config_list* config, const uint8_t* data, size_t size);


int hori_yaml_config_emit_file(const struct hori_yaml_config_list* config, FILE* file);
int hori_yaml_config_emit_string(const struct hori_yaml_config_list* config, uint8_t* data, size_t size, size_t* writeSize);
int hori_yaml_config_emit(const struct hori_yaml_config_list* config, yaml_emitter_t* emitter);
