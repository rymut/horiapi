#pragma once

/** @brief Convert yaml string to boolean

    @param string The text to convert to boolean

    @returns
        This function returns 1 if value is true, 0 on false, -1 if value cannot be converted to bool
 */
int hori_yaml_boolean(const char* string, int string_length);
