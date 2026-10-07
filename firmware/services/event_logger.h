/**
 * @file event_logger.h
 * @brief Circular Event & Audit Logging Service
 * @author Sherifred Singh
 * 
 * Records system transitions, faults, cycles, and operator actions into a
 * structured circular memory buffer with monotonic millisecond timestamps.
 */

#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_LOG_CAPACITY      64U

typedef enum {
    EVENT_SYSTEM_BOOT       = 0x01,
    EVENT_CYCLE_STARTED     = 0x02,
    EVENT_PHASE_CHANGED     = 0x03,
    EVENT_CYCLE_COMPLETED   = 0x04,
    EVENT_CYCLE_ABORTED     = 0x05,
    EVENT_ESTOP_TRIPPED     = 0x06,
    EVENT_FAULT_RAISED      = 0x07,
    EVENT_FAULT_CLEARED     = 0x08,
    EVENT_CONFIG_SAVED      = 0x09,
    EVENT_SERVICE_ENGAGED   = 0x0A,
} event_type_t;

typedef struct {
    uint32_t     timestamp_ms;
    event_type_t event_type;
    uint16_t     param1;
    uint16_t     param2;
} event_record_t;

/**
 * @brief Initialize the event logger.
 */
void event_logger_init(void);

/**
 * @brief Log an event to the circular buffer.
 * @param type Event classification
 * @param param1 Optional context data (e.g. state ID or fault code)
 * @param param2 Optional secondary context data
 */
void event_logger_record(event_type_t type, uint16_t param1, uint16_t param2);

/**
 * @brief Get total number of events recorded since power up.
 */
size_t event_logger_get_total_count(void);

/**
 * @brief Retrieve a recent event by reverse index (0 = latest event).
 * @param index 0 to (min(total, capacity) - 1)
 * @param record Pointer to destination record struct
 * @return true if record exists, false if index out of bounds
 */
bool event_logger_get_entry(size_t index, event_record_t *record);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_LOGGER_H */
