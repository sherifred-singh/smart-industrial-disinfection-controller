/**
 * @file hal_gpio_sim.c
 * @brief Host Simulation Implementation for Digital GPIO
 * @author Sherifred Singh
 */

#include "hal_gpio.h"
#include <string.h>

static hal_gpio_level_t s_gpio_state[HAL_GPIO_PIN_COUNT];

void hal_gpio_init(void)
{
    memset(s_gpio_state, 0, sizeof(s_gpio_state));

    /* Safe default inputs */
    s_gpio_state[HAL_GPIO_IN_START_BTN]       = HAL_GPIO_LEVEL_HIGH; /* Released (Active Low) */
    s_gpio_state[HAL_GPIO_IN_STOP_BTN]        = HAL_GPIO_LEVEL_HIGH; /* Released (Active Low) */
    s_gpio_state[HAL_GPIO_IN_ESTOP_NC]        = HAL_GPIO_LEVEL_HIGH; /* Healthy / Closed */
    s_gpio_state[HAL_GPIO_IN_DOOR_INTERLOCK]  = HAL_GPIO_LEVEL_HIGH; /* Closed */
    s_gpio_state[HAL_GPIO_IN_SERVICE_KEY]     = HAL_GPIO_LEVEL_LOW;  /* Normal operation */
    s_gpio_state[HAL_GPIO_IN_AIRFLOW_SW]      = HAL_GPIO_LEVEL_HIGH; /* Flow OK */

    /* Safe default outputs (all de-energized) */
    s_gpio_state[HAL_GPIO_OUT_DISINFECT_RELAY]= HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_EXHAUST_FAN]    = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_DOOR_LOCK]      = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_STATUS_LED_GRN] = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_STATUS_LED_YEL] = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_STATUS_LED_RED] = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_BUZZER]         = HAL_GPIO_LEVEL_LOW;
    s_gpio_state[HAL_GPIO_OUT_RS485_DE]       = HAL_GPIO_LEVEL_LOW;
}

hal_gpio_level_t hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        return s_gpio_state[pin];
    }
    return HAL_GPIO_LEVEL_LOW;
}

void hal_gpio_write(hal_gpio_pin_t pin, hal_gpio_level_t level)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        s_gpio_state[pin] = level;
    }
}

void hal_gpio_toggle(hal_gpio_pin_t pin)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        s_gpio_state[pin] = (s_gpio_state[pin] == HAL_GPIO_LEVEL_HIGH) ? HAL_GPIO_LEVEL_LOW : HAL_GPIO_LEVEL_HIGH;
    }
}

void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        s_gpio_state[pin] = level;
    }
}
