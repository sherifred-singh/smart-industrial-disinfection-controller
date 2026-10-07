/**
 * @file process_controller.c
 * @brief High-Level Process Coordinator & Supervisory Controller Implementation
 * @author Sherifred Singh
 */

#include "process_controller.h"
#include "app_fsm.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "event_logger.h"
#include "modbus_rtu_slave.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_adc.h"
#include "hal_watchdog.h"

extern bool modbus_rtu_get_start_cmd(void);
extern bool modbus_rtu_get_stop_cmd(void);
extern bool modbus_rtu_get_reset_cmd(void);

void process_controller_init(void)
{
    hal_gpio_init();
    hal_timer_init();
    hal_adc_init();
    hal_watchdog_init(2000U);

    actuator_driver_init();
    sensor_driver_init();

    event_logger_init();
    alarm_fault_init();
    config_manager_init();
    modbus_rtu_init();

    app_fsm_init();
}

void process_controller_step_1ms(void)
{
    modbus_rtu_process();

    if (hal_gpio_read(HAL_GPIO_IN_ESTOP_NC) == HAL_GPIO_LEVEL_LOW) {
        actuator_driver_emergency_all_off();
        app_fsm_dispatch(EVENT_ESTOP_TRIP);
    }
}

void process_controller_step_10ms(void)
{
    sensor_driver_process_10ms(10U);

    sensor_data_t sensors;
    sensor_driver_get_readings(&sensors);

    if (sensors.is_start_btn_pressed || modbus_rtu_get_start_cmd()) {
        app_fsm_dispatch(EVENT_START_CMD);
    }

    if (sensors.is_stop_btn_pressed || modbus_rtu_get_stop_cmd()) {
        app_fsm_dispatch(EVENT_STOP_CMD);
    }

    if (modbus_rtu_get_reset_cmd()) {
        app_fsm_dispatch(EVENT_FAULT_RESET_CMD);
    }

    sensor_driver_clear_button_latches();
}

void process_controller_step_100ms(void)
{
    app_fsm_process(100U);
    alarm_fault_process_100ms(100U);
}

void process_controller_step_1000ms(void)
{
    hal_watchdog_feed();
}
