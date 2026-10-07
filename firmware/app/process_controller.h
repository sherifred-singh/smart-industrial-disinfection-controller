/**
 * @file process_controller.h
 * @brief High-Level Process Coordinator & Supervisory Controller
 * @author Sherifred Singh
 */

#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all system layers (HAL, drivers, services, app FSM).
 */
void process_controller_init(void);

/**
 * @brief 1 Millisecond Task (High-speed Modbus RX/TX, fast safety input scan).
 */
void process_controller_step_1ms(void);

/**
 * @brief 10 Millisecond Task (ADC sampling, filter updates, switch debouncing, FSM dispatch).
 */
void process_controller_step_10ms(void);

/**
 * @brief 100 Millisecond Task (Alarm buzzer sequencing, LED indicators, FSM timer check).
 */
void process_controller_step_100ms(void);

/**
 * @brief 1000 Millisecond Task (Watchdog refresh, telemetry heartbeats, diagnostic checks).
 */
void process_controller_step_1000ms(void);

#ifdef __cplusplus
}
#endif

#endif /* PROCESS_CONTROLLER_H */
