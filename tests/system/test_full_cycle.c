/**
 * @file test_full_cycle.c
 * @brief System Test: Complete Automated Disinfection Cycle Execution
 * @author Sherifred Singh
 */

#include "unity.h"
#include "process_controller.h"
#include "app_fsm.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "event_logger.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_adc.h"

extern void hal_timer_sim_set_manual_mode(bool enable);
extern void hal_timer_sim_advance_ms(uint32_t step_ms);
extern void hal_timer_sim_set_ms(uint32_t ms);
extern void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level);

static void step_system(uint32_t ms_to_run)
{
    uint32_t steps = ms_to_run / 10;
    for (uint32_t i = 0; i < steps; ++i) {
        hal_timer_sim_advance_ms(10);
        process_controller_step_1ms();
        process_controller_step_10ms();
        if ((i % 10) == 0) {
            process_controller_step_100ms();
        }
        if ((i % 100) == 0) {
            process_controller_step_1000ms();
        }
    }
}

void test_system_full_disinfection_cycle(void)
{
    /* 1. Reset configuration to factory defaults for predictable timing */
    config_manager_restore_defaults();

    /* 2. Initialize Full Subsystem */
    process_controller_init();

    /* 3. Enable Deterministic Manual Time Mode */
    hal_timer_sim_set_manual_mode(true);
    hal_timer_sim_set_ms(0);

    TEST_ASSERT_EQUAL_INT(STATE_INIT, app_fsm_get_state());

    /* Run 600ms (Self-test completes in 500ms) */
    step_system(600);
    TEST_ASSERT_EQUAL_INT(STATE_READY, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());
    TEST_ASSERT_FALSE(actuator_driver_is_exhaust_on());

    /* Trigger Start Button (Press low for 40ms, then release) */
    hal_gpio_sim_set_input(HAL_GPIO_IN_START_BTN, HAL_GPIO_LEVEL_LOW);
    step_system(40);
    hal_gpio_sim_set_input(HAL_GPIO_IN_START_BTN, HAL_GPIO_LEVEL_HIGH);
    step_system(10);

    /* Enters PRE_PURGE (default 30s) */
    TEST_ASSERT_EQUAL_INT(STATE_PRE_PURGE, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_exhaust_on());
    TEST_ASSERT_TRUE(actuator_driver_is_door_locked());

    /* Advance 30s -> Enters DISINFECTING (default 300s) */
    step_system(30100);
    TEST_ASSERT_EQUAL_INT(STATE_DISINFECTING, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_disinfect_on());
    TEST_ASSERT_FALSE(actuator_driver_is_exhaust_on());

    /* Advance 300s -> Enters DWELL_SOAK (default 120s) */
    step_system(300100);
    TEST_ASSERT_EQUAL_INT(STATE_DWELL_SOAK, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());
    TEST_ASSERT_FALSE(actuator_driver_is_exhaust_on());

    /* Advance 120s -> Enters POST_PURGE (default 60s) */
    step_system(120100);
    TEST_ASSERT_EQUAL_INT(STATE_POST_PURGE, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_exhaust_on());

    /* Advance 60s -> Enters COMPLETE (5s chime) */
    step_system(60100);
    TEST_ASSERT_EQUAL_INT(STATE_COMPLETE, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_exhaust_on());
    TEST_ASSERT_FALSE(actuator_driver_is_door_locked());

    /* Advance 5s -> Cycles back to STATE_READY */
    step_system(5100);
    TEST_ASSERT_EQUAL_INT(STATE_READY, app_fsm_get_state());

    /* Restore real-time mode */
    hal_timer_sim_set_manual_mode(false);
}

void run_test_full_cycle(void)
{
    RUN_TEST(test_system_full_disinfection_cycle);
}
