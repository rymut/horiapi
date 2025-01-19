#include "hori_yaml_profile.h"

#include <stdarg.h>


#include "hori_yaml_product.h"
#include "hori_input_names.h"
#include "hori_yaml_utils.h"

const char config_profile_id[] = "id";
const char config_profile_layout[] = "layout";
const char config_profile_device[] = "product";
const char config_profile_name[] = "name";
const char config_profile_buttons[] = "buttons";
const char config_profile_button_turbo[] = "turbo";
const char config_profile_button_mapping[] = "map";
const char config_profile_button_inner_dead_zone[] = "inner_dead_zone";
const char config_profile_button_outer_dead_zone[] = "outer_dead_zone";
const char config_profile_button_curve[] = "curve";
const char config_profile_button_curve_movement[] = "movement";
const char config_profile_button_curve_response[] = "response";

#define CONFIG_PROFILE_SCALAR_MAX_LENGHT 80

int hori_yaml_parse_profile_id(yaml_document_t* document, int node_index) {
    if (!document || node_index < 0)
        return -1;
    yaml_node_t* node = yaml_document_get_node(document, node_index);
    if (node == NULL || node->type != YAML_SCALAR_NODE || !hori_yaml_compare_string(node->tag, YAML_INT_TAG))
        return -1;
    return hori_yaml_int(node->data.scalar.value, node->data.scalar.length);
}

int hori_yaml_parse_profile_layout(yaml_document_t* document, int index) {
    if (!document || index < 0)
        return -1;
    yaml_node_t* node = yaml_document_get_node(document, index);
    if (node == NULL || node->type != YAML_SCALAR_NODE || !hori_yaml_compare_string(node->tag, YAML_INT_TAG))
        return -1;
    return hori_yaml_int(node->data.scalar.value, node->data.scalar.length);
}


int hori_yaml_is_string_node(const yaml_node_t* node) {
    if (!node || node->type != YAML_SCALAR_NODE || !hori_yaml_compare_string(node->tag, YAML_INT_TAG))
        return 0;
    return 1;
}

int hori_yaml_is_node(const yaml_node_t* node, int type, const yaml_char_t* tag) {
    if (!node || node->type != type || !hori_yaml_compare_string(node->tag, tag))
        return 0;
    return 1;
}
int hori_yaml_is_map_node(const yaml_node_t* node) {
    if (!node || node->type != YAML_MAPPING_NODE || !hori_yaml_compare_string(node->tag, YAML_MAP_TAG))
        return 0;
    return 1;
}

int hori_yaml_parse_profile_name(yaml_document_t* document, int index, hori_profile_t* profile) {
    if (!document || !profile || index < 0)
        return -1;
    yaml_node_t* node = yaml_document_get_node(document, index);
    if (!node || node->type != YAML_SCALAR_NODE || !hori_yaml_compare_string(node->tag, YAML_STR_TAG))
        return -1;
    return hori_set_profile_name(profile, node->data.scalar.value, node->data.scalar.length);
}

int hori_yaml_parse_profile_button_turbo(yaml_document_t* document, int index, hori_profile_t* profile, int button) {
    if (!document || !profile || button < 0)
        return -1;
    const yaml_node_t* node = yaml_document_get_node(document, index);
    if (!node) {
        if (-1 == hori_set_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_ENABLED, 0))
            return -1;
        if (-1 == hori_set_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_VALUE, 0))
            return -1;
        return 0;
    }
    if (!hori_yaml_is_node(node, YAML_SCALAR_NODE, YAML_INT_TAG))
        return -1;
    int value = hori_yaml_int(node->data.scalar.value, node->data.scalar.length);
    if (-1 == hori_set_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_ENABLED, value != 0))
        return -1;
    if (-1 == hori_set_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_VALUE, value))
        return -1;
    return 0;
}

int hori_yaml_parse_profile_button(yaml_document_t* document, int index, hori_profile_t* profile, int controller, int button) {
    if (!document || !profile || index < 0)
        return -1;
    yaml_node_t* node = yaml_document_get_node(document, index);
    if (!node || node->type != YAML_MAPPING_NODE)
        return -1;

    struct {
        int turbo;
        int mapping;
        int inner_deadzone;
        int outer_deadzone;
        int curve;
    } data = { -1, -1, -1, -1, -1 };
    for (const yaml_node_pair_t* pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; ++pair) {
        yaml_node_t* key_node = yaml_document_get_node(document, pair->key);
        if (YAML_SCALAR_NODE == key_node->type) {
            if (hori_yaml_compare(config_profile_button_turbo, strlen(config_profile_button_turbo), key_node->data.scalar.value, key_node->data.scalar.length))
                data.turbo = pair->value;
            else if (hori_yaml_compare(config_profile_button_mapping, strlen(config_profile_button_mapping), key_node->data.scalar.value, key_node->data.scalar.length))
                data.mapping = pair->value;
            else if (hori_yaml_compare(config_profile_button_inner_dead_zone, strlen(config_profile_button_inner_dead_zone), key_node->data.scalar.value, key_node->data.scalar.length))
                data.inner_deadzone = pair->value;
            else if (hori_yaml_compare(config_profile_button_outer_dead_zone, strlen(config_profile_button_outer_dead_zone), key_node->data.scalar.value, key_node->data.scalar.length))
                data.outer_deadzone = pair->value;
            else if (hori_yaml_compare(config_profile_button_curve, strlen(config_profile_button_curve), key_node->data.scalar.value, key_node->data.scalar.length))
                data.curve = pair->value;
            else
                return -1;
        }
    }
    if (!hori_yaml_parse_profile_button_turbo(document, data.turbo, profile, button)) {
        return -1;
    }
    // button value
    /*
            // serialize fields
        // turbo
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_ENABLED) == 1) {
            const int turbo = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_VALUE);
            if (turbo == -1)
                return 0;
            if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                config_profile_button_turbo, strlen(config_profile_button_turbo), scalar, sizeof(scalar), "%d", turbo))
                return 0;
        }
        // mapping
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_MAPPING_ENABLED) == 1) {
            const int mapping = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_MAPPING_VALUE);
            if (mapping == -1)
                return 0;
            const char* mapping_name = hori_get_button_name(mapping, 4);
            if (mapping_name == NULL)
                return 0;
            if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                config_profile_button_mapping, strlen(config_profile_button_mapping), scalar, sizeof(scalar), "%s", mapping_name))
                return 0;
        }
        // dead zone
        const int inner_dead_zone = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_DEAD_RANGE_VALUE);
        if (inner_dead_zone == -1)
            return 0;
        if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
            config_profile_button_inner_dead_zone, strlen(config_profile_button_inner_dead_zone), scalar, sizeof(scalar), "%d", inner_dead_zone))
            return 0;
        const int outer_dead_zone = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_EDGE_DEAD_RANGE_VALUE);
        if (outer_dead_zone == -1)
            return 0;
        if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
            config_profile_button_outer_dead_zone, strlen(config_profile_button_outer_dead_zone), scalar, sizeof(scalar), "%d", outer_dead_zone))
            return 0;
        // angle and linear
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_LINEAR_ENABLED) != -1) {
            if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_STR_TAG,
                config_profile_button_curve, strlen(config_profile_button_curve), 1, 0, YAML_PLAIN_SCALAR_STYLE))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
            if (!yaml_sequence_start_event_initialize(&event, NULL, YAML_SEQ_TAG, 1, YAML_BLOCK_SEQUENCE_STYLE))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
            struct {
                int movement;
                int response;
            } curve[2];
            curve[0].movement = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_A_VALUE);
            curve[1].movement = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_B_VALUE);
            curve[0].response = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_RESPONSE_A_VALUE);
            curve[1].response = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_RESPONSE_B_VALUE);

            for (int p = 0; p < 2; ++p) {
                if (curve[p].movement == -1 || curve[p].response == -1)
                    return 0;
                if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
                    return 0;
                if (!yaml_emitter_emit(emitter, &event))
                    return 0;
                if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                    config_profile_button_curve_movement, strlen(config_profile_button_curve_movement), scalar, sizeof(scalar), "%d", curve[p].movement))
                    return 0;
                if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                    config_profile_button_curve_response, strlen(config_profile_button_curve_response), scalar, sizeof(scalar), "%d", curve[p].response))
                    return 0;
                if (!yaml_mapping_end_event_initialize(&event))
                    return 0;
                if (!yaml_emitter_emit(emitter, &event))
                    return 0;
            }
            if (!yaml_sequence_end_event_initialize(&event))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
        }
        */
    return 0;
}
int hori_yaml_parse_profile_buttons(yaml_document_t* document, int index, hori_profile_t* profile, int controller) {
    if (!document || !profile || index < 0)
        return -1;
    yaml_node_t* node = yaml_document_get_node(document, index);
    if (!node || node->type != YAML_MAPPING_NODE)
        return -1;
    for (const yaml_node_pair_t* pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; ++pair) {
        const yaml_node_t* key = yaml_document_get_node(document, pair->key);
        const yaml_node_t* value = yaml_document_get_node(document, pair->value);
        if (!hori_yaml_is_string_node(key) || !hori_yaml_is_map_node(value))
            return -1;
        int button_value = hori_get_button_value(key->data.scalar.value, controller);
        if (button_value == -1)
            return -1;
        if (hori_yaml_parse_profile_button(document, index, profile, controller, button_value) == -1)
            return -1;
    }
    return 0;
}

int hori_yaml_parse_config(struct hori_yaml_config* config, const yaml_document_t* document, int node_index, const hori_context_t* context) {
    if (config == NULL || document == NULL || node_index < 0)
        return 0;
    yaml_node_t* map = yaml_document_get_node(document, node_index);
    if (map == NULL || map->type != YAML_MAPPING_NODE)
        return 0;
    // read
    struct {
        int id;
        int layout;
        int device;
        int name;
        int buttons;
    } data = { -1, -1, -1, -1, -1 };
    for (const yaml_node_pair_t* pair = map->data.mapping.pairs.start; pair < map->data.mapping.pairs.top; ++pair) {
        yaml_node_t* key_node = yaml_document_get_node(document, pair->key);
        if (YAML_SCALAR_NODE == key_node->type) {
            if (hori_yaml_compare(config_profile_id, strlen(config_profile_id), key_node->data.scalar.value, key_node->data.scalar.length))
                data.id = pair->value;
            else if (hori_yaml_compare(config_profile_name, strlen(config_profile_name), key_node->data.scalar.value, key_node->data.scalar.length))
                data.name = pair->value;
            else if (hori_yaml_compare(config_profile_device, strlen(config_profile_device), key_node->data.scalar.value, key_node->data.scalar.length))
                data.device = pair->value;
            else if (hori_yaml_compare(config_profile_layout, strlen(config_profile_layout), key_node->data.scalar.value, key_node->data.scalar.length))
                data.layout = pair->value;
            else if (hori_yaml_compare(config_profile_buttons, strlen(config_profile_buttons), key_node->data.scalar.value, key_node->data.scalar.length))
                data.buttons = pair->value;
            else
                return 0;
        }
    }
    // validate
    if (data.device < 0)
        return -1;
    const int product = hori_yaml_parse_product(document, data.device, context);
    if (product == -1)
        return -1;
    hori_profile_t* profile = hori_make_profile(product, context);
    if (!profile)
        return -1;
    const int id = hori_yaml_parse_profile_id(document, data.device);
    const int layout = hori_yaml_parse_profile_layout(document, data.layout);
    if (!hori_yaml_parse_profile_name(document, data.name, config->profile))
        goto error_handling;
    if (!hori_yaml_parse_profile_buttons(document, data.buttons, config->profile, layout))
        goto error_handling;

    config->layout = layout;
    config->profile_id = id;
    config->product = product;
    config->profile = profile;
    return 0;

error_handling:
    hori_free_profile(profile);
    profile = NULL;
    return 1;
}

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
    const char* profile_name = hori_get_profile_name(config->profile);
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
        const char* button_name = hori_get_button_name(button, 4);
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
// http://blog.hypersect.com/interpreting-analog-sticks/
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
    char scalar[CONFIG_PROFILE_SCALAR_MAX_LENGHT] = "";
    if (value) {
        // serialize fields
        // turbo
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_ENABLED) == 1) {
            const int turbo = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_TURBO_VALUE);
            if (turbo == -1)
                return 0;
            if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                config_profile_button_turbo, strlen(config_profile_button_turbo), scalar, sizeof(scalar), "%d", turbo))
                return 0;
        }
        // mapping
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_MAPPING_ENABLED) == 1) {
            const int mapping = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_MAPPING_VALUE);
            if (mapping == -1)
                return 0;
            const char* mapping_name = hori_get_button_name(mapping, 4);
            if (mapping_name == NULL)
                return 0;
            if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                config_profile_button_mapping, strlen(config_profile_button_mapping), scalar, sizeof(scalar), "%s", mapping_name))
                return 0;
        }
        // dead zone
        const int inner_dead_zone = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_DEAD_RANGE_VALUE);
        if (inner_dead_zone == -1)
            return 0;
        if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
            config_profile_button_inner_dead_zone, strlen(config_profile_button_inner_dead_zone), scalar, sizeof(scalar), "%d", inner_dead_zone))
            return 0;
        const int outer_dead_zone = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_EDGE_DEAD_RANGE_VALUE);
        if (outer_dead_zone == -1)
            return 0;
        if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_INT_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
            config_profile_button_outer_dead_zone, strlen(config_profile_button_outer_dead_zone), scalar, sizeof(scalar), "%d", outer_dead_zone))
            return 0;
        // angle and linear
        if (hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_LINEAR_ENABLED) != -1) {
            if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t*)YAML_STR_TAG,
                config_profile_button_curve, strlen(config_profile_button_curve), 1, 0, YAML_PLAIN_SCALAR_STYLE))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
            if (!yaml_sequence_start_event_initialize(&event, NULL, YAML_SEQ_TAG, 1, YAML_BLOCK_SEQUENCE_STYLE))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
            struct {
                int movement;
                int response;
            } curve[2];
            curve[0].movement = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_A_VALUE);
            curve[1].movement = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_MOVEMENT_B_VALUE);
            curve[0].response = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_RESPONSE_A_VALUE);
            curve[1].response = hori_get_profile_button(profile, button, HORI_PROFILE_BUTTON_ANALOG_RESPONSE_B_VALUE);

            for (int p = 0; p < 2; ++p) {
                if (curve[p].movement == -1 || curve[p].response == -1)
                    return 0;
                if (!yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t*)YAML_MAP_TAG, 1, YAML_BLOCK_MAPPING_STYLE))
                    return 0;
                if (!yaml_emitter_emit(emitter, &event))
                    return 0;
                if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                    config_profile_button_curve_movement, strlen(config_profile_button_curve_movement), scalar, sizeof(scalar), "%d", curve[p].movement))
                    return 0;
                if (!hori_yaml_emit_map_scalar(emitter, NULL, YAML_STR_TAG, 1, 0, YAML_PLAIN_SCALAR_STYLE,
                    config_profile_button_curve_response, strlen(config_profile_button_curve_response), scalar, sizeof(scalar), "%d", curve[p].response))
                    return 0;
                if (!yaml_mapping_end_event_initialize(&event))
                    return 0;
                if (!yaml_emitter_emit(emitter, &event))
                    return 0;
            }
            if (!yaml_sequence_end_event_initialize(&event))
                return 0;
            if (!yaml_emitter_emit(emitter, &event))
                return 0;
        }

    }
    if (!yaml_mapping_end_event_initialize(&event))
        return 0;
    if (!yaml_emitter_emit(emitter, &event))
        return 0;
    return 1;
}
