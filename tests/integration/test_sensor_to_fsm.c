/**
 * @file test_sensor_to_fsm.c
 * @brief Integration Test: Sensor Threshold Breach & Interlock Loss -> FSM Trip
 * @author Sherifred Singh
 */

#include "unity.h"
#include "app_fsm.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "alarm_fault_manager.h"
#include "config_manager.h"
#include "event_logger.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_timer.h"

extern void hal_adc_sim_set_raw(hal_adc_channel_t ch, uint16_t raw_count);
extern void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level);

static void setup_integration_env(void)
{
    hal_gpio_init();
    hal_adc_init();
    hal_timer_init();
    alarm_fault_init();
    event_logger_init();
    config_manager_init();
    sensor_driver_init();
    actuator_driver_init();
    app_fsm_init();
}

void test_integration_door_breach_during_active_disinfect(void)
{
    setup_integration_env();

    /* Put system into STATE_DISINFECTING */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter READY */
    app_fsm_dispatch(EVENT_START_CMD);      /* Enter PRE_PURGE */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter DISINFECTING */

    TEST_ASSERT_EQUAL_INT(STATE_DISINFECTING, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_disinfect_on());

    /* Simulate operator opening safety door */
    hal_gpio_sim_set_input(HAL_GPIO_IN_DOOR_INTERLOCK, HAL_GPIO_LEVEL_LOW);
    sensor_driver_process_10ms(10);
    app_fsm_dispatch(EVENT_NONE);

    /* Must immediately trip to EMERGENCY_STOP and kill disinfection relay */
    TEST_ASSERT_EQUAL_INT(STATE_EMERGENCY_STOP, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());
    TEST_ASSERT_TRUE(alarm_fault_has_critical());
}

void test_integration_overtemp_sensor_trip(void)
{
    setup_integration_env();

    /* Start cycle */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter READY */
    app_fsm_dispatch(EVENT_START_CMD);      /* Enter PRE_PURGE */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter DISINFECTING */

    TEST_ASSERT_EQUAL_INT(STATE_DISINFECTING, app_fsm_get_state());

    /* Inject high temperature: 60°C > 45°C limit (1100 mV -> ~1365 counts) */
    hal_adc_sim_set_raw(HAL_ADC_CH_CHAMBER_TEMP, 1400);

    /* Process multiple sensor steps to let digital filter track */
    for (int i = 0; i < 20; ++i) {
        sensor_driver_process_10ms(10);
    }

    app_fsm_dispatch(EVENT_NONE);

    /* Over-temperature fault must be set and FSM must transition to FAULT_LATCH */
    TEST_ASSERT_TRUE(alarm_fault_has_critical());
    TEST_ASSERT_EQUAL_INT(STATE_FAULT_LATCH, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());
}

void run_test_sensor_to_fsm_integration(void)
{
    RUN_TEST(test_integration_door_breach_during_active_disinfect);
    RUN_TEST(test_integration_overtemp_sensor_trip);
}
