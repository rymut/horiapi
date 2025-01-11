#include "hori_input_names.h"

#include <string.h>
#include <horiapi/horiapi.h>

static const char hori_config_button_full_names[][HORI_INPUT_NAME_FULL_LENGTH] = {
    "Not Connected",
    "DPad Up",
    "DPad Down",
    "DPad Left",
    "DPad Right",

    "Left Top",
    "Right Top",

    "Left Stick Center",
    "Right Stick Center",

    "Function Left 1",
    "Function Left 2",
    "Function Right 1",
    "Function Right 2",

    "Left Bottom",
    "Right Bottom",

    "Cross Left",
    "Cross Right",
    "Cross Down",
    "Cross Up",

    "Left Pedal",
    "Right Pedal",

    "Home", // PSBUTTON
    "Select",
    "Start",
    "Media",

    "Left Stick North",
    "Left Stick South",
    "Left Stick West",
    "Left Stick East",
    "Right Stick North",
    "Right Stick South",
    "Right Stick West",
    "Right Stick East",

    "Function Target",
    "Function Agile",

    "Options",
};

static const char hori_config_button_long_names[][HORI_INPUT_NAME_LONG_LENGTH] = {
    "N/C",
    "DP Up",
    "DP Bottom",
    "DP Left",
    "DP Right",
    "L Top",
    "R Top",
    "LS Center",
    "RS Center",
    "FN Left 1",
    "FN Left 2",
    "FN Right 1",
    "FN Right 2",
    "L Buttom",
    "R Buttom",
    "C Left",
    "C Right",
    "C Down",
    "C Up",
    "L Pedal",
    "R Pedal",
    "Home",
    "Select",
    "Start",
    "Media",
    "LS Up",
    "LS Down",
    "LS Left",
    "LS Right",
    "RS Up",
    "RS Down",
    "RS Left",
    "RS Right",
    "FN Agile",
    "FN Target",
    "Options",
};

static const char hori_config_button_short_names[][HORI_INPUT_NAME_SHORT_LENGTH] = {
    "N/C",
    "DPu",
    "DPd",
    "DPl",
    "DPr",
    "Lt",
    "Rt",
    "LSc",
    "RSc",
    "FL1",
    "FL2",
    "FR1",
    "FR2",
    "Lb",
    "Rb",
    "Cl",
    "Cr",
    "Cd",
    "Cu",
    "LP",
    "RP",
    "HOM",
    "SEL",
    "STA",
    "MED",
    "LSn",
    "LSd",
    "LSl",
    "LSr",
    "RSu",
    "RSd",
    "RSl",
    "RSr",
    "Fa",
    "Ft",
    "OPT",
};

static const char hori_ps_axis_full_names[][HORI_INPUT_NAME_FULL_LENGTH] = {
    "Not Available",
    "Left Stick Horizontal",
    "Left Stick Vertical",
    "Right Stick Horizontal",
    "Right Stick Vertical",
    "Left Trigger",
    "Right Trigger",
};

static const char hori_ps_axis_long_names[][HORI_INPUT_NAME_LONG_LENGTH] = {
    "N/A",
    "LSh",
    "LSv",
    "RSh",
    "RSv",
    "LT",
    "RT",
};

static const char hori_ps_axis_short_names[][HORI_INPUT_NAME_SHORT_LENGTH] = {
    "NA",
    "X",
    "Y",
    "Rz",
    "Rx",
    "Ry",
};

const struct hori_input_names button_name_strings[] = {
   {HORI_CONTROLLER_CONFIG, sizeof(hori_config_button_full_names) / sizeof(hori_config_button_full_names[0]), hori_config_button_short_names, hori_config_button_long_names, hori_config_button_full_names},

};

const struct hori_input_names axis_name_strings[] = {
    {HORI_CONTROLLER_CONFIG, sizeof(hori_config_button_full_names) / sizeof(hori_config_button_full_names[0]), hori_config_button_short_names, hori_config_button_long_names, hori_config_button_full_names}, // CONFIG
    {HORI_CONTROLLER_PLAYSTATION4, sizeof(hori_ps_axis_short_names) / sizeof(hori_ps_axis_short_names[0]), hori_ps_axis_short_names, hori_ps_axis_long_names, hori_ps_axis_full_names},
    {HORI_CONTROLLER_PLAYSTATION5, sizeof(hori_ps_axis_short_names) / sizeof(hori_ps_axis_short_names[0]), hori_ps_axis_short_names, hori_ps_axis_long_names, hori_ps_axis_full_names},
    {HORI_CONTROLLER_XINPUT, 0, NULL, NULL, NULL},
    {HORI_CONTROLLER_ANY, 0, NULL, NULL, NULL}, // HID
};

const struct hori_input_names touch_name_strings[] = {
    {HORI_CONTROLLER_ANY, 0, NULL, NULL, NULL},
};

const struct hori_input_names sensor_name_strings[] = {
    {HORI_CONTROLLER_ANY, 0, NULL, NULL, NULL},
};

const char* hori_get_input_name(const struct hori_input_names* item, int input, int lenght) {
    const int controller = HORI_GET_CONTROLLER(input);
    const int index = HORI_BUTTON_INDEX(input);

    if (item == NULL)
        return NULL;
    if (controller & item->controller == item->controller)
        return NULL;
    if (index < 0 || index >= item->count)
        return NULL;
    if (lenght >= HORI_INPUT_NAME_FULL_LENGTH && item->full_names && item->full_names[index])
        return item->full_names[index];
    if (lenght >= HORI_INPUT_NAME_LONG_LENGTH && item->long_names && item->long_names[index])
        return item->long_names[index];
    if (item->short_names && item->short_names[index])
        return item->short_names[index];
    return NULL;
}

const char* hori_find_input_name(struct hori_input_names const* list, int list_size, int input, int length) {
    if (list == NULL)
        return NULL;
    if (input < 0)
        return NULL;
    for (int i = 0; i < list_size; ++i, ++list) {
        const char* name = hori_get_input_name(list, input, length);
        if (name)
            return name;
    }
    return NULL;
}

const char* hori_get_button_name(int button, int lenght) {
    return hori_find_input_name(button_name_strings, sizeof(button_name_strings) / sizeof(button_name_strings[0]), button, lenght);
}

const char* hori_get_axis_name(int axis, int length) {
    return hori_find_input_name(axis_name_strings, sizeof(axis_name_strings) / sizeof(axis_name_strings[0]), axis, length);
}

const char* hori_get_touch_name(int touch, int length) {
    return hori_find_input_name(touch_name_strings, sizeof(touch_name_strings) / sizeof(touch_name_strings[0]), touch, length);
}

const char* hori_get_sensor_name(int sensor, int length) {
    return hori_find_input_name(sensor_name_strings, sizeof(sensor_name_strings) / sizeof(sensor_name_strings[0]), sensor, length);
}

int hori_get_input_value(const struct hori_input_names* object, const char* name, int controller) {
    if (object == NULL)
        return -1;
    if (name == NULL)
        return -1;
    if (controller < 0)
        return -1;
    if (controller == 0)
        controller = object->controller;
    if (controller & object->controller != object->controller)
        return -1;
    for (int i = 0; i < object->count; ++i) {
        if (object->short_names && stricmp(object->short_names[i], name) == 0) {
            return object->controller & i;
        }
        if (object->long_names && stricmp(object->long_names[i], name) == 0) {
            return object->controller & i;
        }
        if (object->full_names && stricmp(object->full_names[i], name) == 0) {
            return object->controller & i;
        }
    }
    return -1;
}

int hori_find_input_value(struct hori_input_names const* list, int list_size, const char* name, int controller) {
    if (list == NULL)
        return -1;
    if (name == NULL)
        return -1;
    if (controller < 0)
        return -1;
    for (int i = 0; i < list_size; ++i, ++list) {
        int input = hori_get_input_value(list, name, controller);
        if (input >= 0) {
            return input;
        }
    }
    return -1;
}

int hori_get_button_value(const char* name, int controller) {
    return hori_find_input_value(button_name_strings, sizeof(button_name_strings) / sizeof(button_name_strings[0]), name, controller);
}

int hori_get_axis_value(const char* name, int controller) {
    return hori_find_input_value(axis_name_strings, sizeof(axis_name_strings) / sizeof(axis_name_strings[0]), name, controller);
}


