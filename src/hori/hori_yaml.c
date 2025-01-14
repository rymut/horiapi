#include "hori_yaml.h"

#include <stdlib.h>
#include <ctype.h>

#include "hori_yaml_profile.h"

struct hori_yaml_config_list* hori_yaml_make_config_list(const hori_device_config_t* device_config, const hori_profile_t* profile) {
    struct hori_yaml_config_list* result = (struct hori_yaml_config_list*)calloc(1, sizeof(struct hori_yaml_config_list));
    if (result == NULL)
        return NULL;
    result->config.layout = device_config->device_config_mode;
    result->config.product = device_config->product;
    result->config.profile = hori_duplicate_profile(profile);
    if (result->config.profile == NULL) {
        free(result);
        result = NULL;
    }
    return result;
}

void hori_yaml_free_config_list(struct hori_yaml_config_list* list) {
    while (list != NULL) {
        struct hori_yaml_config_list* next = list->next;
        hori_free_profile(list->config.profile);
        free(list);
        list = next;
    }
}

int hori_yaml_config_parse_file(struct hori_yaml_config_list* config, FILE* file) {
    if (config == NULL)
        return -1;
    if (file == NULL)
        return -1;
    memset(config, 0, sizeof(struct hori_yaml_config));

    return -1;
}
int hori_yaml_config_parse_string(struct hori_yaml_config_list* config, const uint8_t* data, size_t size) {
    return -1;
}
int hori_yaml_config_emit(const struct hori_yaml_config_list* list, yaml_emitter_t* emitter) {
    yaml_event_t event;
    if (!yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    if (!yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 0))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    if (!yaml_sequence_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_SEQ_TAG, 1, YAML_BLOCK_SEQUENCE_STYLE))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    for (const struct hori_yaml_config_list* item = list; item != NULL; item = item->next) {
        if (!hori_yaml_emit_profile(emitter, &item->config))
            return 0;
    }
    if (!yaml_sequence_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    if (!yaml_document_end_event_initialize(&event, 0))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;

    // end stream
    if (!yaml_stream_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 0;
}

int hori_yaml_config_emit_file(const struct hori_yaml_config_list* config, FILE* file) {
    yaml_emitter_t emitter;
    if (file == NULL)
        return -1;
    if (!yaml_emitter_initialize(&emitter))
        return -1;

    yaml_emitter_set_output_file(&emitter, file);

    int status = hori_yaml_config_emit(config, &emitter);

    yaml_emitter_delete(&emitter);
    return status;
}
int hori_yaml_config_emit_string(const struct hori_yaml_config_list* config, uint8_t* data, size_t size, size_t* size_written) {
    yaml_emitter_t emitter;
    if (data == NULL || size == 0)
        return -1;
    if (!yaml_emitter_initialize(&emitter))
        return -1;

    yaml_emitter_set_output_string(&emitter, data, size, size_written);

    int status = hori_yaml_config_emit(config, &emitter);

    yaml_emitter_delete(&emitter);
    return status;
}

