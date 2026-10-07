/**
 * @file actuator_driver.c
 * @brief Industrial Actuator & Safety Interlock Driver Implementation
 * @author Sherifred Singh
 */

#include "actuator_driver.h"
#include "hal_gpio.h"

static bool s_disinfect_state = false;
static bool s_exhaust_state   = false;
static bool s_door_lock_state = false;

void actuator_driver_init(void)
{
    actuator_driver_emergency_all_off();
}

void actuator_driver_set_disinfect(bool enable)
{
    s_disinfect_state = enable;
    hal_gpio_write(HAL_GPIO_OUT_DISINFECT_RELAY, enable ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_exhaust(bool enable)
{
    s_exhaust_state = enable;
    hal_gpio_write(HAL_GPIO_OUT_EXHAUST_FAN, enable ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_door_lock(bool lock)
{
    s_door_lock_state = lock;
    hal_gpio_write(HAL_GPIO_OUT_DOOR_LOCK, lock ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_led_green(bool on)
{
    hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_GRN, on ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_led_yellow(bool on)
{
    hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_YEL, on ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_led_red(bool on)
{
    hal_gpio_write(HAL_GPIO_OUT_STATUS_LED_RED, on ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_set_buzzer(bool on)
{
    hal_gpio_write(HAL_GPIO_OUT_BUZZER, on ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void actuator_driver_emergency_all_off(void)
{
    s_disinfect_state = false;
    s_exhaust_state   = false;
    s_door_lock_state = false;

    hal_gpio_write(HAL_GPIO_OUT_DISINFECT_RELAY, HAL_GPIO_LEVEL_LOW);
    hal_gpio_write(HAL_GPIO_OUT_EXHAUST_FAN,     HAL_GPIO_LEVEL_LOW);
    hal_gpio_write(HAL_GPIO_OUT_DOOR_LOCK,       HAL_GPIO_LEVEL_LOW);
}

bool actuator_driver_is_disinfect_on(void)
{
    return s_disinfect_state;
}

bool actuator_driver_is_exhaust_on(void)
{
    return s_exhaust_state;
}

bool actuator_driver_is_door_locked(void)
{
    return s_door_lock_state;
}
