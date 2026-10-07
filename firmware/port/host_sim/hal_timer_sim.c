/**
 * @file hal_timer_sim.c
 * @brief Host Simulation Implementation for System Timing & Tick
 * @author Sherifred Singh
 */

#include "hal_timer.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

static uint32_t s_sim_base_ms = 0;
static uint32_t s_manual_tick_ms = 0;
static bool s_use_manual_time = false;

#if defined(_WIN32) || defined(_WIN64)
static uint32_t get_native_ms(void)
{
    return (uint32_t)GetTickCount();
}
#else
static uint32_t get_native_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}
#endif

void hal_timer_init(void)
{
    s_sim_base_ms = get_native_ms();
    s_manual_tick_ms = 0;
    s_use_manual_time = false;
}

uint32_t hal_timer_get_ms(void)
{
    if (s_use_manual_time) {
        return s_manual_tick_ms;
    }
    return (get_native_ms() - s_sim_base_ms);
}

void hal_timer_delay_us(uint32_t us)
{
    (void)us;
}

void hal_timer_delay_ms(uint32_t ms)
{
#if defined(_WIN32) || defined(_WIN64)
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void hal_timer_sim_set_manual_mode(bool enable)
{
    s_use_manual_time = enable;
}

void hal_timer_sim_advance_ms(uint32_t step_ms)
{
    s_manual_tick_ms += step_ms;
}

void hal_timer_sim_set_ms(uint32_t ms)
{
    s_manual_tick_ms = ms;
}
