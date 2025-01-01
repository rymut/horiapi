#include "hori_string_win32.h"

size_t hori_wstring_list_count(const wchar_t *string_list, size_t length) {
    size_t size = 0;
    if (string_list == NULL)
        return size;
    for (size_t len = wcsnlen_s(string_list, length); len != 0 && length != 0; len = wcsnlen_s(string_list, length)) {
        string_list = string_list + len + 1;
        length -= len;
        if (length == 0) {
            break;
        }
        size++;
        length -= 1;
    }
    return size;
}

