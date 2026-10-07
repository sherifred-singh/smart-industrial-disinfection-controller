/**
 * @file hal_watchdog.h
 * @brief Hardware Abstraction Layer for Independent Watchdog (IWDG)
 * @author Sherifred Singh
 * 
 * Hardware-agnostic interface for system supervisory watchdog.
 */

#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the hardware watchdog with a specific timeout window.
 * @param timeout_ms Watchdog timeout period in milliseconds (e.g. 1000ms)
 */
void hal_watchdog_init(uint32_t timeout_ms);

/**
 * @brief Refresh (kick / feed) the watchdog counter.
 * Must only be called if all critical application tasks have executed within bounds.
 */
void hal_watchdog_feed(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_WATCHDOG_H */
