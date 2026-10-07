/**
 * @file app_main.c
 * @brief Main Firmware Application Entry Point & Deterministic Scheduler
 * @author Sherifred Singh
 */

#include "app_main.h"
#include "process_controller.h"
#include "hal_timer.h"

void app_main(void)
{
    process_controller_init();

    uint32_t last_1ms_tick   = hal_timer_get_ms();
    uint32_t last_10ms_tick  = hal_timer_get_ms();
    uint32_t last_100ms_tick = hal_timer_get_ms();
    uint32_t last_1s_tick    = hal_timer_get_ms();

    while (1) {
        uint32_t now = hal_timer_get_ms();

        if ((now - last_1ms_tick) >= 1U) {
            last_1ms_tick = now;
            process_controller_step_1ms();
        }

        if ((now - last_10ms_tick) >= 10U) {
            last_10ms_tick = now;
            process_controller_step_10ms();
        }

        if ((now - last_100ms_tick) >= 100U) {
            last_100ms_tick = now;
            process_controller_step_1000ms(); /* 100ms task */
            process_controller_step_100ms();
        }

        if ((now - last_1s_tick) >= 1000U) {
            last_1s_tick = now;
            process_controller_step_1000ms();
        }
    }
}
