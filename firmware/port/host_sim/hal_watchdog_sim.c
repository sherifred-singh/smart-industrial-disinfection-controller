/**
 * @file hal_watchdog_sim.c
 * @brief Host Simulation Implementation for Watchdog Supervisory
 * @author Sherifred Singh
 */

#include "hal_watchdog.h"

static uint32_t s_wdt_timeout_ms = 1000;
static uint32_t s_wdt_feed_count = 0;
static bool s_wdt_enabled = false;

void hal_watchdog_init(uint32_t timeout_ms)
{
    s_wdt_timeout_ms = timeout_ms;
    s_wdt_feed_count = 0;
    s_wdt_enabled = true;
}

void hal_watchdog_feed(void)
{
    if (s_wdt_enabled) {
        s_wdt_feed_count++;
    }
}

uint32_t hal_watchdog_sim_get_feed_count(void)
{
    return s_wdt_feed_count;
}
