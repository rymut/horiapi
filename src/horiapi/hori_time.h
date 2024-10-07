#pragma once

#include <time.h>

#include <Windows.h>
#pragma comment(lib, "Kernel32")

#if defined(HORI_DOXYGEN)
/** @brief Clock type with miliseconds precission */
typedef struct hori_clock hori_clock_t;

/** @brief Get current time

    @returns
        This function returns current time with milisecond precission
 */
hori_clock_t hori_clock_now();

/** @brief Get clock difference in seconds

    @param clock1 Value returned by @see hori_clock_now
    @param clock2 Value returned by @see hori_clock_now

    @returns
        This function returns amount of of time elapsed between clock1 and clock2 (clock1 - clock2)

    @note
        Returned value will be negative if clock1 < clock2 , and zero if clock1 == clock2
 */
double hori_clock_diff(hori_clock_t clock1, hori_clock_t clock2);

/** @brief Sleep for amount of miliseconds

    @param miliseconds The ammount of milliseconds

    @note
        If @p miliseconds is less or equal to zero function returns immediately
 */
void hori_sleep_ms(int miliseconds);
#else
typedef clock_t hori_clock_t;
#define hori_clock_now() clock()
#define hori_clock_diff(clock1, clock2) (difftime(clock1, clock2) / CLOCKS_PER_SEC)
#define hori_sleep_ms(delay) Sleep(delay)
#endif
