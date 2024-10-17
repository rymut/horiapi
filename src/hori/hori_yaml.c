#include "hori_yaml.h"
#include <stdlib.h>
struct hori_config_audio* hori_yaml_parse_audio(yaml_parser_t* parser);

struct hori_config_audio* hori_yaml_parse_file(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        return NULL;
    }
    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser)) {
        fclose(file);
        return NULL;
    }
    yaml_parser_set_input_file(&parser, file);
    struct hori_config_audio* result = hori_yaml_parse_audio(&parser);
    yaml_parser_delete(&parser);
    fclose(file);
    return result;
}

#include <ctype.h>
/** @brief Convert yaml string to boolean

    @param string The text to convert to boolean

    @returns
        This function returns 1 if value is true, 0 on false, -1 if value cannot be converted to bool
 */
int hori_yaml_get_boolean(const char* string, int string_length)
{
    // this is not YAML reference correct
    static const char* t[6] = { NULL, "y", "on", "yes", "true", NULL };
    static const char* f[6] = { NULL, "n", "no", "off", NULL, "false" };
    char lowercase[5] = { 0, 0, 0, 0, 0 };
    if (string == NULL) {
        return -1;
    }
    if (string_length <= 0 || string_length > 5) {
        return -1;
    }
    for (int i = 0; i < string_length; ++i) {
        lowercase[i] = tolower(string[i]);
    }
    if (t[string_length] != NULL && strncmp(lowercase, t[string_length], string_length) == 0) {
        return 1;
    }
    if (f[string_length] != NULL && strncmp(lowercase, f[string_length], string_length) == 0) {
        return 1;
    }
    return -1;
}

struct hori_parser_unknown_state {
    int depth;
    int return_state;
};
void hori_yaml_parse_unknown(yaml_parser_t* parser) {
    if (parser == NULL) {
        return;
    }
    yaml_event_t event;
    int depth = 0;
    while (yaml_parser_parse(parser, &event)) {
        switch (event.type) {
        case YAML_STREAM_END_EVENT:
        case YAML_DOCUMENT_END_EVENT:
        case YAML_MAPPING_END_EVENT:
        case YAML_SEQUENCE_END_EVENT:
            depth--;
            break;
        case YAML_STREAM_START_EVENT:
        case YAML_DOCUMENT_START_EVENT:
        case YAML_SEQUENCE_START_EVENT:
        case YAML_MAPPING_START_EVENT:
            depth++;
            break;
        default:
            break;
        }
        yaml_event_delete(&event);
        if (depth == 0) {
            break;
        }
    }
}
enum hori_parse_audio_state {
    HORI_PARSE_AUDIO_STATE_START,
    HORI_PARSE_AUDIO_STATE_KEY_UNKNOWN,

    HORI_PARSE_AUDIO_STATE_ENABLED_KEY,
    HORI_PARSE_AUDIO_STATE_SPEAKER_KEY,
    HORI_PARSE_AUDIO_STATE_SPEAKER_SCALAR, // empty scalar is valid
    HORI_PARSE_AUDIO_STATE_SPEAKER_MAP_START,
    HORI_PARSE_AUDIO_STATE_SPEAKER_MAP_END,
    HORI_PARSE_AUDIO_STATE_SPEAKER_VOLUME_KEY,
    HORI_PARSE_AUDIO_STATE_SPEAKER_VOLUME_SCALAR,
    HORI_PARSE_AUDIO_STATE_SPEAKER_MIXER_KEY,
    HORI_PARSE_AUDIO_STATE_SPEAKER_MIXER_SCALAR,

    HORI_PARSE_AUDIO_STATE_MICROPHONE_KEY,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_SCALAR,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MAP_START,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MAP_END,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MUTED_KEY,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MUTED_SCALAR,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MUTED_SENSITIVITY_KEY,
    HORI_PARSE_AUDIO_STATE_MICROPHONE_MUTED_SENSITIVITY_SCALAR,
    HORI_PARSE_AUDIO_STATE_SKIP,
    HORI_PARSE_AUDIO_STATE_STOP, // after end of block
};

/** @brief Parse yaml audio map need to be called at the after MAP_START
 */
struct hori_config_audio* hori_yaml_parse_audio(yaml_parser_t* parser) {
    struct hori_config_audio* config = (struct hori_config_audio*)calloc(1, sizeof(struct hori_config_audio));
    if (config == NULL) {
        return NULL;
    }
    yaml_event_t event;
    int status = 1;
    int state = HORI_PARSE_AUDIO_STATE_START;
    int depth = 0;
    do {
        if (!yaml_parser_parse(parser, &event)) {
            printf("Parser error %d\n", parser->error);
            free(config);
            return NULL;
        }
        char* value = NULL;
        switch (state) {
        case HORI_PARSE_AUDIO_STATE_START:
            switch (event.type) {
            case YAML_NO_EVENT:
                break;
            case YAML_MAPPING_END_EVENT:
                state = HORI_PARSE_AUDIO_STATE_STOP;
                break;
            case YAML_SCALAR_EVENT:
                value = (char*)event.data.scalar.value;
                if (strcmp(value, "enabled") == 0) {
                    state = HORI_PARSE_AUDIO_STATE_ENABLED_KEY;
                }
                else if (strcmp(value, "speaker") == 0) {
                    state = HORI_PARSE_AUDIO_STATE_SPEAKER_KEY;
                }
                else if (strcmp(value, "microphone") == 0) {
                    state = HORI_PARSE_AUDIO_STATE_MICROPHONE_KEY;
                }
                else {
                    state = HORI_PARSE_AUDIO_STATE_KEY_UNKNOWN;
                }
                break;
            default:

            }
            break;
        case HORI_PARSE_AUDIO_STATE_KEY_UNKNOWN:
            switch (event.type) {
            case YAML_SCALAR_EVENT:
            case YAML_ALIAS_EVENT:
                if (depth == 0) {
                    state = HORI_PARSE_AUDIO_STATE_START;
                }
                break;
            case YAML_MAPPING_END_EVENT:
            case YAML_SEQUENCE_END_EVENT:
                depth--;
                break;
            case YAML_SEQUENCE_START_EVENT:
            case YAML_MAPPING_START_EVENT:
                depth++;
                break;
            default:
                break;
            }
        }
        switch (event.type)
        {

        case YAML_NO_EVENT: puts("No event!"); break;
            /* Stream start/end */
        case YAML_STREAM_START_EVENT: puts("STREAM START"); break;
        case YAML_STREAM_END_EVENT:   puts("STREAM END");   break;
            /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT: puts("<b>Start Document</b>"); break;
            // not realy valid??
        case YAML_DOCUMENT_END_EVENT:   puts("<b>End Document</b>");   break;
            // should exit
        case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
        case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
        case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
        case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");    break;
            /* Data */
        case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
        case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); break;
        }
        if (event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while (event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);
    return config;
}
