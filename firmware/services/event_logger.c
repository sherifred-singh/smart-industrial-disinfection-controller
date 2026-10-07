/**
 * @file event_logger.c
 * @brief Circular Event & Audit Logging Service Implementation
 * @author Sherifred Singh
 */

#include "event_logger.h"
#include "hal_timer.h"
#include <string.h>

static event_record_t s_log_buffer[EVENT_LOG_CAPACITY];
static size_t         s_head_index   = 0;
static size_t         s_total_events = 0;

void event_logger_init(void)
{
    memset(s_log_buffer, 0, sizeof(s_log_buffer));
    s_head_index   = 0;
    s_total_events = 0;

    event_logger_record(EVENT_SYSTEM_BOOT, 0, 0);
}

void event_logger_record(event_type_t type, uint16_t param1, uint16_t param2)
{
    s_log_buffer[s_head_index].timestamp_ms = hal_timer_get_ms();
    s_log_buffer[s_head_index].event_type   = type;
    s_log_buffer[s_head_index].param1       = param1;
    s_log_buffer[s_head_index].param2       = param2;

    s_head_index = (s_head_index + 1) % EVENT_LOG_CAPACITY;
    s_total_events++;
}

size_t event_logger_get_total_count(void)
{
    return s_total_events;
}

bool event_logger_get_entry(size_t index, event_record_t *record)
{
    if (record == NULL || index >= EVENT_LOG_CAPACITY || index >= s_total_events) {
        return false;
    }

    size_t actual_idx;
    if (s_head_index > index) {
        actual_idx = (s_head_index - 1) - index;
    } else {
        actual_idx = (EVENT_LOG_CAPACITY + s_head_index - 1) - index;
    }

    *record = s_log_buffer[actual_idx];
    return true;
}
