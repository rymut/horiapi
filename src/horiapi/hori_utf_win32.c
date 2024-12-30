#include "hori_utf.h"
#if defined(WIN32)

#include <Windows.h>
#include <stringapiset.h>
#pragma comment(lib, "Kernel32.lib")

wchar_t* hori_internal_UTF8toUTF16(const char* utf8) {
    wchar_t* utf16 = NULL;
    int result = 0, error = 0;
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, NULL, 0);
    if (len) {
        utf16 = (wchar_t*)calloc(len, sizeof(wchar_t));
        if (utf16 == NULL) {
            return NULL;
        }
        result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, utf16, len);
        error = GetLastError();
    }
    return utf16;
}

char* hori_internal_UTF16toUTF8(const wchar_t* utf16)
{
    char* utf8 = NULL;
    int len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16, -1, NULL, 0, NULL, NULL);
    if (len) {
        utf8 = (char*)calloc(len, sizeof(char));
        if (utf8 == NULL) {
            return NULL;
        }
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16, -1, utf8, len, NULL, NULL);
    }
    return utf8;
}

#endif // defined(WIN32)
