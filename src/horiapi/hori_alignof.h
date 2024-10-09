#pragma once

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
#define HORI_ALIGNOF(type_name) (sizeof(struct { char prefix; type_name value; char suffix; })/3)
#endif

#endif /* !defined(HORI_DOXYGEN) */
