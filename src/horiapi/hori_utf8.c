#include "hori_utf8.h"

#include "../utf8/utf8.h"

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
int hori_internal_utf8_byte_size(uint8_t const* data, int size) {
    if (data == NULL || size < 0) {
        return -1;
    }
    uint32_t code_point = 0;
    uint32_t state = UTF8_ACCEPT;
    int byte_count = 0;
    for (byte_count = 0; byte_count < size && *data; ++byte_count) {
        if (decode(&state, &code_point, *data) == UTF8_REJECT) {
            return -1;
        }
    }
    return state == UTF8_REJECT ? -1 : byte_count;
}
