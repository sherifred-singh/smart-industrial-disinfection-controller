/**
 * @file alarm_fault_manager.c
 * @brief Industrial Alarm & Fault Management Service Implementation
 * @author Sherifred Singh
 */

#include "alarm_fault_manager.h"
#include "hal_gpio.h"

typedef struct {
    uint32_t         active_faults;
    fault_severity_t severities[16];
    uint32_t         cadence_counter_ms;
} fault_state_t;

static fault_state_t s_fault_state;

void alarm_fault_init(void)
{
    s_fault_state.active_faults      = FAULT_CODE_NONE;
    s_fault_state.cadence_counter_ms = 0;
    for (int i = 0; i < 16; ++i) {
        s_fault_state.severities[i] = FAULT_SEV_NONE;
    }
}

void alarm_fault_set(fault_code_t code, fault_severity_t severity)
{
    s_fault_state.active_faults |= (uint32_t)code;

    for (uint8_t i = 0; i < 16; ++i) {
        if ((code & (1U << i)) != 0) {
            if (severity > s_fault_state.severities[i]) {
                s_fault_state.severities[i] = severity;
            }
        }
    }
}

void alarm_fault_clear(fault_code_t code)
{
    s_fault_state.active_faults &= ~((uint32_t)code);
    for (uint8_t i = 0; i < 16; ++i) {
        if ((code & (1U << i)) != 0) {
            s_fault_state.severities[i] = FAULT_SEV_NONE;
        }
    }
}

bool alarm_fault_reset_all(void)
{
    if (hal_gpio_read(HAL_GPIO_IN_ESTOP_NC) == HAL_GPIO_LEVEL_LOW) {
        s_fault_state.active_faults |= (uint32_t)FAULT_CODE_ESTOP_ACTIVE;
        s_fault_state.severities[0] = FAULT_SEV_CRITICAL;
        return false;
    }

    s_fault_state.active_faults = FAULT_CODE_NONE;
    for (int i = 0; i < 16; ++i) {
        s_fault_state.severities[i] = FAULT_SEV_NONE;
    }

    hal_gpio_write(HAL_GPIO_OUT_BUZZER, HAL_GPIO_LEVEL_LOW);
    hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_RED, HAL_GPIO_LEVEL_LOW);

    return true;
}

bool alarm_fault_has_critical(void)
{
    if (s_fault_state.active_faults == 0) {
        return false;
    }

    for (uint8_t i = 0; i < 16; ++i) {
        if ((s_fault_state.active_faults & (1U << i)) != 0) {
            if (s_fault_state.severities[i] == FAULT_SEV_CRITICAL) {
                return true;
            }
        }
    }
    return false;
}

bool alarm_fault_has_warning(void)
{
    if (s_fault_state.active_faults == 0) {
        return false;
    }

    for (uint8_t i = 0; i < 16; ++i) {
        if ((s_fault_state.active_faults & (1U << i)) != 0) {
            if (s_fault_state.severities[i] == FAULT_SEV_WARNING) {
                return true;
            }
        }
    }
    return false;
}

uint32_t alarm_fault_get_active_mask(void)
{
    return s_fault_state.active_faults;
}

fault_severity_t alarm_fault_get_highest_severity(void)
{
    fault_severity_t highest = FAULT_SEV_NONE;
    for (uint8_t i = 0; i < 16; ++i) {
        if ((s_fault_state.active_faults & (1U << i)) != 0) {
            if (s_fault_state.severities[i] > highest) {
                highest = s_fault_state.severities[i];
            }
        }
    }
    return highest;
}

void alarm_fault_process_100ms(uint32_t elapsed_ms)
{
    s_fault_state.cadence_counter_ms += elapsed_ms;

    if (alarm_fault_has_critical()) {
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_RED, HAL_GPIO_LEVEL_HIGH);
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_GRN, HAL_GPIO_LEVEL_LOW);
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_YEL, HAL_GPIO_LEVEL_LOW);

        bool beep_phase = ((s_fault_state.cadence_counter_ms % 400U) < 200U);
        hal_gpio_write(HAL_GPIO_OUT_BUZZER, beep_phase ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
    } else if (alarm_fault_has_warning()) {
        bool blink_phase = ((s_fault_state.cadence_counter_ms % 1000U) < 500U);
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_YEL, blink_phase ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_RED, HAL_GPIO_LEVEL_LOW);

        bool chirp_phase = ((s_fault_state.cadence_counter_ms % 2000U) < 100U);
        hal_gpio_write(HAL_GPIO_OUT_BUZZER, chirp_phase ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
    } else {
        hal_gpio_write(HAL_GPIO_OUT_BUZZER, HAL_GPIO_LEVEL_LOW);
        hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_RED, HAL_GPIO_LEVEL_LOW);
    }
}
