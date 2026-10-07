/**
 * @file actuator_driver.h
 * @brief Industrial Actuator & Safety Interlock Driver
 * @author Sherifred Singh
 * 
 * Manages power relays for disinfection generation, chamber scrubber exhaust,
 * electromagnetic door interlock solenoid, and beacon indicators.
 */

#ifndef ACTUATOR_DRIVER_H
#define ACTUATOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all actuator outputs in de-energized fail-safe state.
 */
void actuator_driver_init(void);

/**
 * @brief Command Disinfection Actuator (Relay 1).
 * @param enable true = Energize, false = De-energize
 */
void actuator_driver_set_disinfect(bool enable);

/**
 * @brief Command Chamber Exhaust Scrubber Fan (Relay 2).
 * @param enable true = Run fan, false = Stop fan
 */
void actuator_driver_set_exhaust(bool enable);

/**
 * @brief Command Electromagnetic Safety Door Lock Solenoid.
 * @param lock true = Lock door (prevent opening), false = Unlock
 */
void actuator_driver_set_door_lock(bool lock);

/**
 * @brief Set Status Beacon Indicators.
 */
void actuator_driver_set_led_green(bool on);
void actuator_driver_set_led_yellow(bool on);
void actuator_driver_set_led_red(bool on);
void actuator_driver_set_buzzer(bool on);

/**
 * @brief Immediate Hardware Emergency Cutoff (Forces all load relays OFF).
 * Must be executed in <10 microseconds upon E-Stop or door breach.
 */
void actuator_driver_emergency_all_off(void);

/**
 * @brief Readback actuator states.
 */
bool actuator_driver_is_disinfect_on(void);
bool actuator_driver_is_exhaust_on(void);
bool actuator_driver_is_door_locked(void);

#ifdef __cplusplus
}
#endif

#endif /* ACTUATOR_DRIVER_H */
