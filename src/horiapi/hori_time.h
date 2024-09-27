#pragma once

#include <time.h>

typedef clock_t hori_clock_t;

#define hori_clock_now() clock()

#define hori_clock_diff(prev, now) (difftime(prev, now) / CLOCKS_PER_SEC)

#define hori_sleep_ms(delay) Sleep(delay)
