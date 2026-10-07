/**
 * @file alarm_fault_manager.h
 * @brief Industrial Alarm & Fault Management Service
 * @author Sherifred Singh
 * 
 * Manages fault classification, active latches, audible buzzer patterns,
 * status beacon sequencing, and safe-state emergency trip signaling.
 */

#ifndef ALARM_FAULT_MANAGER_H
#define ALARM_FAULT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fault Severity Levels
 */
typedef enum {
    FAULT_SEV_NONE     = 0,
    FAULT_SEV_INFO     = 1, /**< Operational notice (e.g. process finished) */
    FAULT_SEV_WARNING  = 2, /**< Non-critical (e.g. high ambient temp), cycle continues */
    FAULT_SEV_CRITICAL = 3  /**< Critical safety trip -> Immediate actuator cut & latch */
} fault_severity_t;

/**
 * @brief System Fault Code Bitmask / Enumeration
 */
typedef enum {
    FAULT_CODE_NONE                 = 0,
    FAULT_CODE_ESTOP_ACTIVE         = (1U << 0), /**< Emergency stop contact open */
    FAULT_CODE_DOOR_BREACH          = (1U << 1), /**< Safety door opened during active cycle */
    FAULT_CODE_TEMP_OVER_LIMIT      = (1U << 2), /**< Chamber temp exceeded maximum limit */
    FAULT_CODE_AMBIENT_GAS_LEAK     = (1U << 3), /**< External leak sensor detected threshold breach */
    FAULT_CODE_AIRFLOW_LOST         = (1U << 4), /**< Exhaust scrubber airflow switch dropped during purge */
    FAULT_CODE_PROCESS_TIMEOUT      = (1U << 5), /**< Cycle exceeded maximum allowable time envelope */
    FAULT_CODE_NVM_CORRUPTED        = (1U << 6), /**< Configuration CRC check failed in EEPROM */
    FAULT_CODE_COMM_TIMEOUT         = (1U << 7), /**< Modbus master supervisory heartbeat lost */
    FAULT_CODE_SENSOR_DISCONNECTED  = (1U << 8), /**< ADC open-circuit / short-circuit detected */
} fault_code_t;

/**
 * @brief Initialize the Alarm & Fault Manager.
 */
void alarm_fault_init(void);

/**
 * @brief Raise a specific fault code with severity.
 * @param code Fault bitmask flag
 * @param severity Severity classification
 */
void alarm_fault_set(fault_code_t code, fault_severity_t severity);

/**
 * @brief Clear a non-latched warning fault.
 * @param code Fault bitmask flag
 */
void alarm_fault_clear(fault_code_t code);

/**
 * @brief Attempt to acknowledge and reset all latched faults (requires physical reset command).
 * @return true if all critical faults were successfully cleared, false if fault condition persists.
 */
bool alarm_fault_reset_all(void);

/**
 * @brief Check if any critical safety fault is currently active.
 * @return true if system must remain in SAFE_SHUTDOWN state.
 */
bool alarm_fault_has_critical(void);

/**
 * @brief Check if any warning is currently active.
 */
bool alarm_fault_has_warning(void);

/**
 * @brief Get the active fault bitmask.
 */
uint32_t alarm_fault_get_active_mask(void);

/**
 * @brief Get the highest current active severity.
 */
fault_severity_t alarm_fault_get_highest_severity(void);

/**
 * @brief Periodic 100ms task to drive buzzer beep cadences and status LED beacon patterns.
 * @param elapsed_ms Elapsed tick time (e.g. 100ms)
 */
void alarm_fault_process_100ms(uint32_t elapsed_ms);

#ifdef __cplusplus
}
#endif

#endif /* ALARM_FAULT_MANAGER_H */
