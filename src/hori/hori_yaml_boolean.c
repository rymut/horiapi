#include "hori_yaml_boolean.h"

#include <stdlib.h>

int hori_yaml_boolean(const char* string, int string_length)
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

