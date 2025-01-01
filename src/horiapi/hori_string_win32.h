#pragma once
#include <wchar.h>

/** @brief String list */
struct hori_wstring_list {
    wchar_t *value;
    size_t count;
    size_t length;
};

inline struct hori_wstring_list hori_empty_wstring_list() {
    struct hori_wstring_list empty = { NULL, 0, 0 };
    return empty;
}

inline void hori_init_wstring_list(struct hori_wstring_list* wstring_list) {
    if (wstring_list == 0)
        return;
    wstring_list->value = NULL;
    wstring_list->count = 0;
    wstring_list->length = 0;
}

inline void hori_clear_wstring_list(struct hori_wstring_list* wstring_list) {
    if (wstring_list) {
        free(wstring_list->value);
        wstring_list->value = NULL;
        wstring_list->count = 0;
        wstring_list->length = 0;
    }
}

/** @brief Get number of elements in string list

    @param list[in] The string list containing at least @p byte_size elements
    @param byte_size[in] The length of list

    @returns
        Returns number of elements in string list (strings ending with \0) inside memory until empty string is found.
  */
size_t hori_wstring_list_count(const wchar_t *list, size_t length);

