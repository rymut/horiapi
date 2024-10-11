#pragma once

#define _HORI_CONCAT_IMPL(prefix, suffix) prefix##suffix
#define _HORI_CONCAT(prefix, suffix) _HORI_CONCAT_IMPL(prefix, suffix)

#if defined(__COUNTER__)
#define _HORI_UNIQUE_INDEX __COUNTER__
#elif defined(__LINE__)
#define _HORI_UNIQUE_INDEX __LINE__
#endif
