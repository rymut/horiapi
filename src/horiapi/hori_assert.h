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
#define _HORI_CAT(a, ...) _HORI_PRIMITIVE_CAT(a, __VA_ARGS__)
#define _HORI_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define _HORI_IIF(c) _HORI_PRIMITIVE_CAT(_HORI_IIF_, c)
#define _HORI_IIF_0(t, ...) __VA_ARGS__
#define _HORI_IIF_1(t, ...) t

#define _HORI_CONCAT_IMPL(prefix, suffix) prefix##suffix
#define _HORI_CONCAT(prefix, suffix) _HORI_CONCAT_IMPL(prefix, suffix)

#if !defined(_HORI_STATIC_ASSERT_TYPE_NAME)
#define _HORI_STATIC_ASSERT_TYPE_NAME _hori_compile_time_assert_
#endif // !defined(_HORI_STATIC_ASSERT_TYPE_NAME)
#ifndef _HORI_STATIC_ASSERT_TYPE_PREFIX
#define _HORI_STATIC_ASSERT_TYPE_PREFIX 
#endif

#if defined(__COUNTER__)
#define _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(prefix) _HORI_CONCAT(_HORI_CONCAT(prefix, _HORI_STATIC_ASSERT_TYPE_NAME), __COUNTER__)
#else
#define _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(prefix) _HORI_CONCAT(_HORI_CONCAT(prefix, _HORI_STATIC_ASSERT_TYPE_NAME), __LINE__)
#endif

// evaluate if VA_ARGS_IS_EMPTY
// https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
#define _HORI_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define _HORI_VA_ARGS_HAS_COMMA(...) _HORI_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _HORI_PARENTHESIS(...) ,
#define _HORI_VA_ARGS_IS_EMPTY(...)                                                    \
_HORI_VA_ARGS_IS_EMPTY_IMPL(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          _HORI_VA_ARGS_HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          _HORI_VA_ARGS_HAS_COMMA(_HORI_PARENTHESIS __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          _HORI_VA_ARGS_HAS_COMMA(__VA_ARGS__ (/*empty*/)),                           \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          _HORI_VA_ARGS_HAS_COMMA(_HORI_PARENTHESIS_ __VA_ARGS__ (/*empty*/))      \
          )

#define _HORI_VA_ARGS_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define _HORI_VA_ARGS_IS_EMPTY_IMPL(_0, _1, _2, _3) _HORI_VA_ARGS_HAS_COMMA(_HORI_VA_ARGS_PASTE5(_HORI_VA_ARGS_IS_EMPTY_CASE_0001, _0, _1, _2, _3))
#define _HORI_VA_ARGS_IS_EMPTY_CASE_0001 ,

#if !defined(_HORI_STATIC_ASSERT_FIELD_NAME)
#define _HORI_STATIC_ASSERT_FIELD_NAME static_assert_fail
#endif

#define _HORI_VA_ARG_0(value, ...) value
#define _HORI_STATIC_ASSERT_MAKE_FIELD_NAME(_expr, ...) _HORI_IIF(_HORI_VA_ARGS_IS_EMPTY(__VA_ARGS__))(_HORI_STATIC_ASSERT_FIELD_NAME, _HORI_VA_ARG_0(__VA_ARGS__, NULL))

#define _HORI_STATIC_ASSERT(expr, ...) \
    struct _HORI_STATIC_ASSERT_MAKE_STRUCT_NAME(_HORI_STATIC_ASSERT_TYPE_PREFIX) { \
        int _HORI_STATIC_ASSERT_MAKE_FIELD_NAME(expr, __VA_ARGS__): sizeof(int) * 8 + !(expr); \
    }

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
// C23 - supports static_assert(expr, msg) && static_assert(expr)
#define HORI_STATIC_ASSERT(expr, ...) static_assert(expr, __VA_ARGS__)
#define HORI_STATIC_ASSERT(expr, ...) \
    _HORI_IIF(_HORI_VA_ARGS_IS_EMPTY(__VA_ARGS__))( \
            static_assert(expr), \
            static_assert(expr, _HORI_STR(_HORI_VA_ARG_0(__VA_ARGS__, NULL))) \
        )
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
// C11 - supports static_assert(expr, msg) when message is missing str of expr is used
#define HORI_STATIC_ASSERT(expr, ...) \
    _Static_assert((expr), \
        _HORI_IIF(_HORI_VA_ARGS_IS_EMPTY(__VA_ARGS__))( \
            _HORI_STR(expr), \
            _HORI_STR(_HORI_VA_ARG_0(__VA_ARGS__, NULL)) \
        ) \
    )
#else 
#define HORI_STATIC_ASSERT(expr, ...) _HORI_STATIC_ASSERT(expr, __VA_ARGS__)
#endif //

#endif // !defined(HORI_DOXYGEN)
