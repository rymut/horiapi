#include "hori_yaml_utils.h"

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

int hori_yaml_compare_string(const char* a, const char* b) {
    return a != NULL && b != NULL && strcmp(a, b);
}

int hori_yaml_compare(const char* a, size_t a_size, yaml_char_t* b, size_t b_size) {
    if (a_size != b_size || a_size < 0 || b_size < 0)
        return 0;
    if (a == NULL || b == NULL || a_size == 0)
        return a == b;
    return strncmp(a, b, a_size);
}
