#pragma once

#if defined(WIN32)
#include <Windows.h>
#pragma comment(lib, "Kernel32")

#if !defined(hori_sleep_ms)
#define hori_sleep_ms(delay) Sleep(delay)
#endif // !defined(hori_sleep_ms) 
#endif // defined(WIN32)
