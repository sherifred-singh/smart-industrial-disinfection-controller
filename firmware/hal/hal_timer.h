/**
 * @file hal_timer.h
 * @brief Hardware Abstraction Layer for System Tick & Timing
 * @author Sherifred Singh
 * 
 * Provides monotonic millisecond tick counters and microsecond delays for
 * non-blocking task scheduling, debouncing, timeouts, and state elapsed time.
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the hardware timer peripheral (e.g. SysTick / TIM2).
 */
void hal_timer_init(void);

/**
 * @brief Get the current monotonic system uptime in milliseconds.
 * @return Milliseconds since system startup (handles 32-bit rollover safely).
 */
uint32_t hal_timer_get_ms(void);

/**
 * @brief Check if a specified time duration has elapsed without blocking.
 * @param start_ms Reference timestamp from hal_timer_get_ms()
 * @param interval_ms Target duration in milliseconds
 * @return true if duration has elapsed, false otherwise
 */
static inline bool hal_timer_has_elapsed(uint32_t start_ms, uint32_t interval_ms)
{
    return ((hal_timer_get_ms() - start_ms) >= interval_ms);
}

/**
 * @brief Microsecond busy-wait delay (for microsecond sensor protocols e.g. 1-Wire).
 * @param us Microseconds to delay
 */
void hal_timer_delay_us(uint32_t us);

/**
 * @brief Millisecond busy-wait delay (strictly for low-level peripheral init sequences).
 * @param ms Milliseconds to delay
 */
void hal_timer_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TIMER_H */
