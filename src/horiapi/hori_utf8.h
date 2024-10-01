#pragma once

#include <inttypes.h>

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
