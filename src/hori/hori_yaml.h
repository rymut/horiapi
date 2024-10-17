#pragma once

#include <inttypes.h>

#include <yaml.h>

struct hori_config {
    int product; // product number
    struct hori_config_profile_list* list;
};

struct hori_config_audio {
    int enabled;
    int volume_level;
    int volume_mixer;
    int mic_muted;
    int mic_sensivity;
};
struct hori_config_feedback {
    uint8_t feedback[6];
};
struct hori_profile_name {
    wchar_t name[32];
};
// see https://github.com/meffie/libyaml-examples/blob/master/parse.c
struct hori_config_profile {
    int id; // unique number per mode

    int dplsrs; 
    struct hori_profile_name *name; // 
    struct hori_audio_config *audio;
    struct hori_feedback_config *feedback;
    struct hori_sensor_list* sensors;
    struct hori_button_config_list* buttons;
    struct hori_stick_config_list* sticks;
    struct hori_wheel_config* wheel;
};

struct hori_config_audio* hori_yaml_parse_file(const char* fileName);

struct hori_config_sensor {
    uint8_t sensor[4]; // ?? how big
};
struct hori_sensor_list {
    struct hori_config_sensor sensor;
    struct hori_sensor_list* next;
};
struct hori_config_profile_list {
    struct hori_config_profile config;
    struct hori_config_profile_list* next;
};

