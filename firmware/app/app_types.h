/**
 * @file app_types.h
 * @brief Application Data Types & State Machine Definitions
 * @author Sherifred Singh
 */

#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Primary System State Machine States
 */
typedef enum {
    STATE_INIT = 0,             /**< Hardware bring-up & self-test */
    STATE_IDLE,                 /**< Standby, Green LED ON, door open/idle */
    STATE_READY,                /**< Door closed & interlocks satisfied, ready for start */
    STATE_PRE_PURGE,            /**< Exhaust ventilation active, verifying airflow */
    STATE_DISINFECTING,         /**< Disinfection active, chamber locked, real-time monitoring */
    STATE_DWELL_SOAK,           /**< Chemical dwell/soak contact time */
    STATE_POST_PURGE,           /**< Exhaust scrubber fan active, ventilating chamber */
    STATE_COMPLETE,             /**< Cycle finished successfully, door unlocked */
    STATE_WARNING,              /**< Transient warning condition */
    STATE_FAULT_LATCH,          /**< Critical fault -> Immediate safe shutdown & latch */
    STATE_EMERGENCY_STOP,       /**< E-Stop pressed / Safety interlock breached */
    STATE_SERVICE_MODE,         /**< Manual technician service / calibration mode */
    STATE_COUNT
} app_state_t;

/**
 * @brief State Machine Event Stimuli
 */
typedef enum {
    EVENT_NONE = 0,
    EVENT_START_CMD,            /**< Pushbutton or Modbus start command */
    EVENT_STOP_CMD,             /**< Pushbutton or Modbus stop command */
    EVENT_ESTOP_TRIP,           /**< Emergency stop switch opened */
    EVENT_DOOR_OPEN,            /**< Chamber door opened */
    EVENT_DOOR_CLOSE,           /**< Chamber door closed */
    EVENT_TIMER_EXPIRED,        /**< Current phase duration reached */
    EVENT_CRITICAL_FAULT,       /**< Critical sensor/hardware fault */
    EVENT_FAULT_RESET_CMD,      /**< Manual fault acknowledge */
    EVENT_SERVICE_ENTER,        /**< Service keyswitch turned ON */
    EVENT_SERVICE_EXIT          /**< Service keyswitch turned OFF */
} app_event_t;

#ifdef __cplusplus
}
#endif

#endif /* APP_TYPES_H */
