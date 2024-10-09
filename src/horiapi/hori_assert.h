#pragma once

#include <assert.h>

#if defined(HORI_DOXYGEN)
/** @brief Complie time assertion

    @since 0.1.0

    @param expr The expression to test
    @param ... The optional message - must be passed without "" and any spaces

    @code{.c}
        // this will fail and generate error "one_is_not_equal_to_one"
        HORI_STATIC_ASSERT(1 == 1, one_is_not_equal_to_one)
    @endcode

    @note
        Compilation will fail if @p expr is evaluated as false

 */
void HORI_STATIC_ASSERT(int expr, ...);

/** @brief Compile time assertion

*/
#else
#define _HORI_STR_IMPL(a) #a
#define _HORI_STR(a) _HORI_STR_IMPL(a)
#define _HORI_CAT(a, b) _HORI_PRIMITIVE_CAT(a, b)
#define _HORI_PRIMITIVE_CAT(a, b) a ## b

#define _HORI_CONCAT_IMPL(prefix, suffix) prefix##suffix
#define _HORI_CONCAT(prefix, suffix) _HORI_CONCAT_IMPL(prefix, suffix)

#if !defined(_HORI_STATIC_ASSERT_TYPE_NAME)
#define _HORI_STATIC_ASSERT_TYPE_NAME hori_compile_time_assert_
#endif /* !defined(_HORI_STATIC_ASSERT_TYPE_NAME) */
#ifndef _HORI_STATIC_ASSERT_TYPE_PREFIX
#define _HORI_STATIC_ASSERT_TYPE_PREFIX _
#endif

#if defined(__COUNTER__)
#define _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(prefix) _HORI_CONCAT(_HORI_CONCAT(prefix, _HORI_STATIC_ASSERT_TYPE_NAME), __COUNTER__)
#else
#define _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(prefix) _HORI_CONCAT(_HORI_CONCAT(prefix, _HORI_STATIC_ASSERT_TYPE_NAME), __LINE__)
#endif


#if !defined(_HORI_STATIC_ASSERT_FIELD_NAME)
#define _HORI_STATIC_ASSERT_FIELD_NAME static_assert_fail
#endif

#define _HORI_STATIC_ASSERT(expr, message) \
    struct _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(_HORI_STATIC_ASSERT_TYPE_PREFIX) { \
        int _HORI_STATIC_ASSERT_FIELD_NAME: sizeof(int) * 8 + !(expr); \
    }

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* C23 - supports static_assert(expr, msg) && static_assert(expr) */
#define HORI_STATIC_ASSERT(expr, message) \
            static_assert(expr, message)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 - supports static_assert(expr, msg) when message is missing str of expr is used */
#define HORI_STATIC_ASSERT(expr, message) \
    _Static_assert((expr), message)
#else 
#define HORI_STATIC_ASSERT(expr, message) _HORI_STATIC_ASSERT(expr, message)
#endif

#endif /* !defined(HORI_DOXYGEN) */
