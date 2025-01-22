#pragma once

#include <yaml.h>

/** @brief Convert yaml string to boolean

    @param string The text to convert to boolean

    @returns
        This function returns 1 if value is true, 0 on false, -1 if value cannot be converted to bool
 */
int hori_yaml_boolean(const char* string, int string_length);

/** @brief Convert yaml string to integer

    @params[out] value The converted value (can be null)
    @params[in] string The array of characters (must end with \0 character)
    @params[in] string_length The length of array (without \0 character)

    @summary
        This function returns @p string converted to integer (plain integer, !!int <plain>) or 0 if conversion fails

    @returns
        This function returns 0 on failure
  */
int hori_yaml_int(int *value, const char* string, int string_length);

/** @brief Compare null terminated strings

    @param[in] a The first string
    @param[in] b The second string

    @returns
        The function returns 0 if strings are equal
  */
int hori_yaml_compare_string(const char* a, const char* b);

/** @brief Compare length based strings */
int hori_yaml_compare(const char* a, size_t a_size, yaml_char_t* b, size_t b_size);
