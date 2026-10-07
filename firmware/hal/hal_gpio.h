/**
 * @file hal_gpio.h
 * @brief Hardware Abstraction Layer for Digital GPIO
 * @author Sherifred Singh
 * 
 * Hardware-agnostic interface for digital input/output lines including
 * actuator relays, safety interlock inputs, switches, status LEDs, and alarms.
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logical GPIO Pin Identifiers
 */
typedef enum {
    /* Digital Inputs (Active Low with Pull-Up or Active High) */
    HAL_GPIO_IN_START_BTN = 0,    /**< Cycle Start Pushbutton (Active Low) */
    HAL_GPIO_IN_STOP_BTN,         /**< Cycle Stop Pushbutton (Active Low) */
    HAL_GPIO_IN_ESTOP_NC,         /**< Emergency Stop (Normally Closed contact, 0 = Tripped) */
    HAL_GPIO_IN_DOOR_INTERLOCK,   /**< Safety Enclosure Interlock (1 = Closed, 0 = Open) */
    HAL_GPIO_IN_SERVICE_KEY,      /**< Manual Service Mode Keyswitch (1 = Active) */
    HAL_GPIO_IN_AIRFLOW_SW,       /**< Exhaust Airflow Differential Switch (1 = Flow OK) */

    /* Digital Outputs (Relay & Actuator Drivers) */
    HAL_GPIO_OUT_DISINFECT_RELAY, /**< Disinfection Generator / UV-C / Aerosol Relay */
    HAL_GPIO_OUT_EXHAUST_FAN,     /**< Chamber Scrubber / Exhaust Ventilation Fan */
    HAL_GPIO_OUT_DOOR_LOCK,       /**< Electromagnetic Safety Door Lock */
    HAL_GPIO_OUT_STATUS_LED_GRN,  /**< System Normal / Idle Status LED */
    HAL_GPIO_OUT_STATUS_LED_YEL,  /**< Process Active / Warning Status LED */
    HAL_GPIO_OUT_STATUS_LED_RED,  /**< System Fault / Emergency Stop LED */
    HAL_GPIO_OUT_BUZZER,          /**< Audible Alarm Buzzer */
    HAL_GPIO_OUT_RS485_DE,        /**< RS-485 Transceiver Driver Enable (DE/!RE) */

    HAL_GPIO_PIN_COUNT
} hal_gpio_pin_t;

/**
 * @brief GPIO Pin Logic Level
 */
typedef enum {
    HAL_GPIO_LEVEL_LOW  = 0,
    HAL_GPIO_LEVEL_HIGH = 1
} hal_gpio_level_t;

/**
 * @brief Initialize all GPIO peripherals and configure safe default output states.
 */
void hal_gpio_init(void);

/**
 * @brief Read the current digital logic level of a specified input/output pin.
 * @param pin Logical pin identifier
 * @return HAL_GPIO_LEVEL_LOW or HAL_GPIO_LEVEL_HIGH
 */
hal_gpio_level_t hal_gpio_read(hal_gpio_pin_t pin);

/**
 * @brief Set the output logic level of a specified output pin.
 * @param pin Logical pin identifier
 * @param level Desired logic level
 */
void hal_gpio_write(hal_gpio_pin_t pin, hal_gpio_level_t level);

/**
 * @brief Toggle the output state of a specified output pin.
 * @param pin Logical pin identifier
 */
void hal_gpio_toggle(hal_gpio_pin_t pin);

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H */
