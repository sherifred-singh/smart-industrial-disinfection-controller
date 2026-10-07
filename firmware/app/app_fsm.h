/**
 * @file app_fsm.h
 * @brief Finite State Machine Engine for Process Sequencing & Safety Supervisory
 * @author Sherifred Singh
 */

#ifndef APP_FSM_H
#define APP_FSM_H

#include "app_types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the FSM and enter STATE_INIT.
 */
void app_fsm_init(void);

/**
 * @brief Dispatch an event stimulus into the FSM.
 * @param event Event to process
 */
void app_fsm_dispatch(app_event_t event);

/**
 * @brief Execute periodic FSM tick (called every 10ms/100ms from main loop).
 * @param elapsed_ms Step time in milliseconds
 */
void app_fsm_process(uint32_t elapsed_ms);

/**
 * @brief Get the current FSM state.
 */
app_state_t app_fsm_get_state(void);

/**
 * @brief Get the name string of a given state (for CLI & logging).
 */
const char *app_fsm_state_to_str(app_state_t state);

/**
 * @brief Get elapsed time in current active phase (in seconds).
 */
uint32_t app_fsm_get_phase_elapsed_sec(void);

/**
 * @brief Get remaining time in current active phase (in seconds).
 */
uint32_t app_fsm_get_phase_remaining_sec(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_FSM_H */
