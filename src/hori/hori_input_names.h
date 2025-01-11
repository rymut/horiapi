#pragma once


#define HORI_INPUT_NAME_FULL_LENGTH 25
#define HORI_INPUT_NAME_SHORT_LENGTH 4
#define HORI_INPUT_NAME_LONG_LENGTH 13

struct hori_input_names {
    int controller;
    int count;
    const char (*short_names)[HORI_INPUT_NAME_SHORT_LENGTH];
    const char (*long_names)[HORI_INPUT_NAME_LONG_LENGTH];
    const char (*full_names)[HORI_INPUT_NAME_FULL_LENGTH];
};

/** @brief Get platform dependent button name

    @param[in] button The button name
    @param[in] length The maximum length of input

    @returns
        This function returns NULL if name cannot be determined with length, otherwise name with \0
 */
const char* hori_get_button_name(int button, int length);

const char* hori_get_axis_name(int axis, int lenght);

const char* hori_get_touch_name(int touch, int length);

const char* hori_get_sensor_name(int sensor, int length);

/** @brief Get name from input index
 */
const char* hori_get_input_name(const struct hori_input_names* item, int input, int lenght);

/** @brief Find input name
 */
const char* hori_find_input_name(struct hori_input_names const* list, int list_size, int input, int length);

int hori_get_button_value(const char* name, int controller);
int hori_get_axis_value(const char* name, int controller);

int hori_get_input_value(const struct hori_input_names* object, const char* name, int controller);

int hori_find_input_value(struct hori_input_names const* list, int list_size, const char* name, int controller);

