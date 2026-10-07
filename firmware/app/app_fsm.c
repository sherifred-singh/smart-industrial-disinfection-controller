/**
 * @file app_fsm.c
 * @brief Finite State Machine Engine Implementation
 * @author Sherifred Singh
 */

#include "app_fsm.h"
#include "actuator_driver.h"
#include "sensor_driver.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "event_logger.h"
#include "hal_timer.h"

typedef struct {
    app_state_t current_state;
    app_state_t previous_state;
    uint32_t    state_entry_ms;
    uint32_t    target_duration_ms;
} fsm_context_t;

static fsm_context_t s_fsm;

static const char *const s_state_names[STATE_COUNT] = {
    [STATE_INIT]           = "INIT",
    [STATE_IDLE]           = "IDLE",
    [STATE_READY]          = "READY",
    [STATE_PRE_PURGE]      = "PRE_PURGE",
    [STATE_DISINFECTING]   = "DISINFECTING",
    [STATE_DWELL_SOAK]     = "DWELL_SOAK",
    [STATE_POST_PURGE]     = "POST_PURGE",
    [STATE_COMPLETE]       = "COMPLETE",
    [STATE_WARNING]        = "WARNING",
    [STATE_FAULT_LATCH]    = "FAULT_LATCH",
    [STATE_EMERGENCY_STOP] = "EMERGENCY_STOP",
    [STATE_SERVICE_MODE]   = "SERVICE_MODE"
};

const char *app_fsm_state_to_str(app_state_t state)
{
    if (state < STATE_COUNT) {
        return s_state_names[state];
    }
    return "UNKNOWN";
}

static void transition_to(app_state_t new_state)
{
    if (s_fsm.current_state == new_state) {
        return;
    }

    const config_data_t *cfg = config_manager_get();

    /* 1. State Exit Actions */
    switch (s_fsm.current_state) {
        case STATE_DISINFECTING:
            actuator_driver_set_disinfect(false);
            break;
        case STATE_PRE_PURGE:
        case STATE_POST_PURGE:
            actuator_driver_set_exhaust(false);
            break;
        case STATE_COMPLETE:
            actuator_driver_set_door_lock(false);
            actuator_driver_set_buzzer(false);
            break;
        default:
            break;
    }

    s_fsm.previous_state = s_fsm.current_state;
    s_fsm.current_state  = new_state;
    s_fsm.state_entry_ms = hal_timer_get_ms();

    event_logger_record(EVENT_PHASE_CHANGED, (uint16_t)s_fsm.previous_state, (uint16_t)new_state);

    /* 2. State Entry Actions & Target Duration Setup */
    switch (new_state) {
        case STATE_INIT:
            s_fsm.target_duration_ms = 500U;
            actuator_driver_emergency_all_off();
            break;

        case STATE_IDLE:
            s_fsm.target_duration_ms = 0;
            actuator_driver_set_disinfect(false);
            actuator_driver_set_exhaust(false);
            actuator_driver_set_door_lock(false);
            actuator_driver_set_led_green(true);
            actuator_driver_set_led_yellow(false);
            actuator_driver_set_led_red(false);
            break;

        case STATE_READY:
            s_fsm.target_duration_ms = 0;
            actuator_driver_set_disinfect(false);
            actuator_driver_set_exhaust(false);
            actuator_driver_set_door_lock(false);
            actuator_driver_set_led_green(true);
            actuator_driver_set_led_yellow(false);
            actuator_driver_set_led_red(false);
            break;

        case STATE_PRE_PURGE:
            s_fsm.target_duration_ms = (uint32_t)cfg->pre_purge_duration_s * 1000U;
            actuator_driver_set_door_lock(true);
            actuator_driver_set_exhaust(true);
            actuator_driver_set_led_green(false);
            actuator_driver_set_led_yellow(true);
            event_logger_record(EVENT_CYCLE_STARTED, 0, 0);
            break;

        case STATE_DISINFECTING:
            s_fsm.target_duration_ms = (uint32_t)cfg->disinfect_duration_s * 1000U;
            actuator_driver_set_exhaust(false);
            actuator_driver_set_disinfect(true);
            actuator_driver_set_door_lock(true);
            actuator_driver_set_led_yellow(true);
            break;

        case STATE_DWELL_SOAK:
            s_fsm.target_duration_ms = (uint32_t)cfg->dwell_soak_duration_s * 1000U;
            actuator_driver_set_disinfect(false);
            actuator_driver_set_exhaust(false);
            actuator_driver_set_door_lock(true);
            break;

        case STATE_POST_PURGE:
            s_fsm.target_duration_ms = (uint32_t)cfg->post_purge_duration_s * 1000U;
            actuator_driver_set_disinfect(false);
            actuator_driver_set_exhaust(true);
            actuator_driver_set_door_lock(true);
            break;

        case STATE_COMPLETE:
            s_fsm.target_duration_ms = 5000U;
            actuator_driver_set_disinfect(false);
            actuator_driver_set_exhaust(false);
            actuator_driver_set_door_lock(false);
            actuator_driver_set_led_green(true);
            actuator_driver_set_led_yellow(false);
            event_logger_record(EVENT_CYCLE_COMPLETED, 0, 0);
            break;

        case STATE_EMERGENCY_STOP:
            actuator_driver_emergency_all_off();
            if (cfg->auto_purge_on_fault) {
                actuator_driver_set_exhaust(true);
            }
            actuator_driver_set_led_green(false);
            actuator_driver_set_led_red(true);
            event_logger_record(EVENT_ESTOP_TRIPPED, 0, 0);
            break;

        case STATE_FAULT_LATCH:
            actuator_driver_emergency_all_off();
            if (cfg->auto_purge_on_fault) {
                actuator_driver_set_exhaust(true);
            }
            actuator_driver_set_led_green(false);
            actuator_driver_set_led_red(true);
            event_logger_record(EVENT_FAULT_RAISED, (uint16_t)alarm_fault_get_active_mask(), 0);
            break;

        case STATE_SERVICE_MODE:
            s_fsm.target_duration_ms = 0;
            actuator_driver_emergency_all_off();
            event_logger_record(EVENT_SERVICE_ENGAGED, 0, 0);
            break;

        default:
            break;
    }
}

void app_fsm_init(void)
{
    s_fsm.current_state      = STATE_INIT;
    s_fsm.previous_state     = STATE_INIT;
    s_fsm.state_entry_ms     = hal_timer_get_ms();
    s_fsm.target_duration_ms = 500U;

    actuator_driver_emergency_all_off();
}

void app_fsm_dispatch(app_event_t event)
{
    sensor_data_t sensors;
    sensor_driver_get_readings(&sensors);
    const config_data_t *cfg = config_manager_get();

    /* Global Safety Overrides */
    if (event == EVENT_ESTOP_TRIP || !sensors.is_estop_normal) {
        if (s_fsm.current_state != STATE_EMERGENCY_STOP) {
            alarm_fault_set(FAULT_CODE_ESTOP_ACTIVE, FAULT_SEV_CRITICAL);
            transition_to(STATE_EMERGENCY_STOP);
            return;
        }
    }

    if (alarm_fault_has_critical() && s_fsm.current_state != STATE_FAULT_LATCH && s_fsm.current_state != STATE_EMERGENCY_STOP) {
        transition_to(STATE_FAULT_LATCH);
        return;
    }

    switch (s_fsm.current_state) {
        case STATE_INIT:
            if (event == EVENT_TIMER_EXPIRED) {
                if (sensors.is_service_mode) {
                    transition_to(STATE_SERVICE_MODE);
                } else if (sensors.is_door_closed || !cfg->door_interlock_enable) {
                    transition_to(STATE_READY);
                } else {
                    transition_to(STATE_IDLE);
                }
            }
            break;

        case STATE_IDLE:
            if (sensors.is_service_mode) {
                transition_to(STATE_SERVICE_MODE);
            } else if (sensors.is_door_closed || !cfg->door_interlock_enable) {
                transition_to(STATE_READY);
            }
            break;

        case STATE_READY:
            if (sensors.is_service_mode) {
                transition_to(STATE_SERVICE_MODE);
            } else if (!sensors.is_door_closed && cfg->door_interlock_enable) {
                transition_to(STATE_IDLE);
            } else if (event == EVENT_START_CMD) {
                transition_to(STATE_PRE_PURGE);
            }
            break;

        case STATE_PRE_PURGE:
            if (event == EVENT_STOP_CMD) {
                transition_to(STATE_READY);
            } else if (!sensors.is_door_closed && cfg->door_interlock_enable) {
                alarm_fault_set(FAULT_CODE_DOOR_BREACH, FAULT_SEV_CRITICAL);
                transition_to(STATE_EMERGENCY_STOP);
            } else if (!sensors.is_airflow_ok) {
                alarm_fault_set(FAULT_CODE_AIRFLOW_LOST, FAULT_SEV_CRITICAL);
                transition_to(STATE_FAULT_LATCH);
            } else if (event == EVENT_TIMER_EXPIRED) {
                transition_to(STATE_DISINFECTING);
            }
            break;

        case STATE_DISINFECTING:
            if (event == EVENT_STOP_CMD) {
                transition_to(STATE_POST_PURGE);
            } else if (!sensors.is_door_closed && cfg->door_interlock_enable) {
                alarm_fault_set(FAULT_CODE_DOOR_BREACH, FAULT_SEV_CRITICAL);
                transition_to(STATE_EMERGENCY_STOP);
            } else if (event == EVENT_TIMER_EXPIRED) {
                transition_to(STATE_DWELL_SOAK);
            }
            break;

        case STATE_DWELL_SOAK:
            if (event == EVENT_STOP_CMD) {
                transition_to(STATE_POST_PURGE);
            } else if (!sensors.is_door_closed && cfg->door_interlock_enable) {
                alarm_fault_set(FAULT_CODE_DOOR_BREACH, FAULT_SEV_CRITICAL);
                transition_to(STATE_EMERGENCY_STOP);
            } else if (event == EVENT_TIMER_EXPIRED) {
                transition_to(STATE_POST_PURGE);
            }
            break;

        case STATE_POST_PURGE:
            if (event == EVENT_TIMER_EXPIRED) {
                transition_to(STATE_COMPLETE);
            }
            break;

        case STATE_COMPLETE:
            if (event == EVENT_TIMER_EXPIRED || event == EVENT_START_CMD || event == EVENT_STOP_CMD) {
                if (sensors.is_door_closed || !cfg->door_interlock_enable) {
                    transition_to(STATE_READY);
                } else {
                    transition_to(STATE_IDLE);
                }
            }
            break;

        case STATE_EMERGENCY_STOP:
        case STATE_FAULT_LATCH:
            if (event == EVENT_FAULT_RESET_CMD) {
                if (alarm_fault_reset_all()) {
                    event_logger_record(EVENT_FAULT_CLEARED, 0, 0);
                    if (sensors.is_door_closed || !cfg->door_interlock_enable) {
                        transition_to(STATE_READY);
                    } else {
                        transition_to(STATE_IDLE);
                    }
                }
            }
            break;

        case STATE_SERVICE_MODE:
            if (!sensors.is_service_mode) {
                if (sensors.is_door_closed || !cfg->door_interlock_enable) {
                    transition_to(STATE_READY);
                } else {
                    transition_to(STATE_IDLE);
                }
            }
            break;

        default:
            break;
    }
}

void app_fsm_process(uint32_t elapsed_ms)
{
    (void)elapsed_ms;

    sensor_data_t sensors;
    sensor_driver_get_readings(&sensors);
    const config_data_t *cfg = config_manager_get();

    /* Auto-transition between IDLE and READY based on door status */
    if (s_fsm.current_state == STATE_IDLE) {
        if (sensors.is_service_mode) {
            transition_to(STATE_SERVICE_MODE);
        } else if (sensors.is_door_closed || !cfg->door_interlock_enable) {
            transition_to(STATE_READY);
        }
    } else if (s_fsm.current_state == STATE_READY) {
        if (sensors.is_service_mode) {
            transition_to(STATE_SERVICE_MODE);
        } else if (!sensors.is_door_closed && cfg->door_interlock_enable) {
            transition_to(STATE_IDLE);
        }
    }

    if (s_fsm.target_duration_ms > 0) {
        if (hal_timer_has_elapsed(s_fsm.state_entry_ms, s_fsm.target_duration_ms)) {
            app_fsm_dispatch(EVENT_TIMER_EXPIRED);
        }
    }
}

app_state_t app_fsm_get_state(void)
{
    return s_fsm.current_state;
}

uint32_t app_fsm_get_phase_elapsed_sec(void)
{
    uint32_t elapsed_ms = hal_timer_get_ms() - s_fsm.state_entry_ms;
    return (elapsed_ms / 1000U);
}

uint32_t app_fsm_get_phase_remaining_sec(void)
{
    if (s_fsm.target_duration_ms == 0) {
        return 0;
    }

    uint32_t elapsed_ms = hal_timer_get_ms() - s_fsm.state_entry_ms;
    if (elapsed_ms >= s_fsm.target_duration_ms) {
        return 0;
    }
    return ((s_fsm.target_duration_ms - elapsed_ms) / 1000U);
}
