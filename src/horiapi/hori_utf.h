#pragma once

#include <inttypes.h>
#include <wchar.h>

/** @brief Validate UTF-8 sequence

    @since 0.1.0
    @param data The utf-8 decoded string (\0 string ending not required)
    @param size The maximum length of string (in bytes)

    @returns
        This function returns length in bytes of UTF8 sequence (without \0 character),
        or -1 if sequence is invalid

    @note
        This function will return -1 on invalid input arguments
  */
int hori_internal_utf8_byte_size(uint8_t const* data, int size);

/** @brief Convert utf-8 to UTF16

    @param utf8[in] The input utf8 string

    @returns
        The function returns UTF16 string, or NULL on error
        User is responsible for freeing returned string.
 */
wchar_t* hori_internal_UTF8toUTF16(const char* utf8);

/** @brief Convert utf16 to utf8

    @param utf16[in] The input utf16 string

    @returns
        The function returns UTF8 string, or NULL on error.
        User is responsible for freeing returned string.
  */
char* hori_internal_UTF16toUTF8(const wchar_t* utf16);
