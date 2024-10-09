#pragma once

#include <assert.h>

#if defined(HORI_DOXYGEN)
/** @brief Compile time assertion

    @since 0.1.0

    @param expr The expression to test
    @param message The message shown when assert fails

    @code{.c}
        // this will fail and should generate error "one_is_not_equal_to_one"
        HORI_STATIC_ASSERT(1 == 1, "one is not equal to one")
    @endcode

    @note
        Compilation will fail if @p expr is evaluated as false

 */
void HORI_STATIC_ASSERT(int expr, const char *message);

/** @brief Compile time assertion

*/
#else

#define _HORI_CONCAT_IMPL(prefix, suffix) prefix##suffix
#define _HORI_CONCAT(prefix, suffix) _HORI_CONCAT_IMPL(prefix, suffix)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* C23 - supports static_assert(expr, msg) && static_assert(expr) */
#define HORI_STATIC_ASSERT(expr, message) \
            static_assert(expr, message)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 - supports static_assert(expr, msg) when message is missing str of expr is used */
#define HORI_STATIC_ASSERT(expr, message) \
    _Static_assert((expr), message)
#else
#if defined(__COUNTER__)
    #define _HORI_STATIC_ASSERT_INDEX __COUNTER__
#elif defined(__LINE__)
    #define _HORI_STATIC_ASSERT_INDEX __LINE__
#endif
#if defined(_HORI_STATIC_ASSERT_INDEX)
    #define HORI_STATIC_ASSERT(expr, message) \
        struct _HORI_CONCAT(_static_assertion_test_, _HORI_STATIC_ASSERT_INDEX) { \
            int static_assertion_failed: sizeof(int) * 8 + !(expr); \
        }
#else
    #define HORI_STATIC_ASSERT(expr, message)
#endif
#endif

#endif /* !defined(HORI_DOXYGEN) */
