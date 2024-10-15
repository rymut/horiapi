#pragma once

#include "hori_macros.h"

#if defined(HORI_DOXYGEN)
/** @brief Get type aligment

    @since 0.1.0

    @param type_name The type to compute aligment

    @code{.c}
        struct custom_struct { char  a; int b; char c; };
        HORI_ALIGNOF(struct custom_struct);
    @endcode
 */
void HORI_ALIGNOF(type_name);

#else

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* C23 - supports static_assert(expr, msg) && static_assert(expr) */
#define HORI_ALIGNOF(type_name) alignof(type_name)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 - supports _Alignof(type_name) when message is missing str of expr is used */
#define HORI_ALIGNOF(type_name) _Alignof(type_name)
#else
#if defined(_HORI_UNIQUE_INDEX)
#define HORI_ALIGNOF(type_name) ((sizeof( \
    struct _HORI_CONCAT(_hori_alignof_, _HORI_UNIQUE_INDEX) { \
        char prefix; \
        type_name value; \
        char suffix; \
    }) - sizeof(type_name))/2)
#else
/* Will work but will generate warning on some compilers */
#define HORI_ALIGNOF(type_name) (sizeof( \
    struct { \
        char prefix; \
        type_name value; \
        char suffix; \
    })/3)
#endif
#endif

#endif /* !defined(HORI_DOXYGEN) */
