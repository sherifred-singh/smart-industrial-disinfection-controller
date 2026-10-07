/**
 * @file test_app_fsm.c
 * @brief Unit Tests for Process State Machine Transitions
 * @author Sherifred Singh
 */

#include "unity.h"
#include "app_fsm.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "event_logger.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_timer.h"

extern void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level);

static void setup_fsm_test_env(void)
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

void test_app_fsm_startup_to_ready(void)
{
    setup_fsm_test_env();
    TEST_ASSERT_EQUAL_INT(STATE_INIT, app_fsm_get_state());

    /* Complete init timer */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED);
    TEST_ASSERT_EQUAL_INT(STATE_READY, app_fsm_get_state());
}

void test_app_fsm_normal_cycle_progression(void)
{
    setup_fsm_test_env();
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter READY */
    TEST_ASSERT_EQUAL_INT(STATE_READY, app_fsm_get_state());

    /* From READY -> Dispatch START_CMD */
    app_fsm_dispatch(EVENT_START_CMD);
    TEST_ASSERT_EQUAL_INT(STATE_PRE_PURGE, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_exhaust_on());
    TEST_ASSERT_TRUE(actuator_driver_is_door_locked());

    /* Pre-purge timer expires -> Enter DISINFECTING */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED);
    TEST_ASSERT_EQUAL_INT(STATE_DISINFECTING, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_disinfect_on());

    /* Disinfection timer expires -> Enter DWELL_SOAK */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED);
    TEST_ASSERT_EQUAL_INT(STATE_DWELL_SOAK, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());

    /* Dwell expires -> Enter POST_PURGE */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED);
    TEST_ASSERT_EQUAL_INT(STATE_POST_PURGE, app_fsm_get_state());
    TEST_ASSERT_TRUE(actuator_driver_is_exhaust_on());

    /* Post-purge expires -> Enter COMPLETE */
    app_fsm_dispatch(EVENT_TIMER_EXPIRED);
    TEST_ASSERT_EQUAL_INT(STATE_COMPLETE, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_door_locked());
}

void test_app_fsm_emergency_stop_override(void)
{
    setup_fsm_test_env();
    app_fsm_dispatch(EVENT_TIMER_EXPIRED); /* Enter READY */
    app_fsm_dispatch(EVENT_START_CMD);      /* Enter PRE_PURGE */
    TEST_ASSERT_EQUAL_INT(STATE_PRE_PURGE, app_fsm_get_state());

    /* Trip E-Stop Contact */
    hal_gpio_sim_set_input(HAL_GPIO_IN_ESTOP_NC, HAL_GPIO_LEVEL_LOW);
    app_fsm_dispatch(EVENT_ESTOP_TRIP);

    TEST_ASSERT_EQUAL_INT(STATE_EMERGENCY_STOP, app_fsm_get_state());
    TEST_ASSERT_FALSE(actuator_driver_is_disinfect_on());
    TEST_ASSERT_TRUE(alarm_fault_has_critical());
}

void run_test_app_fsm(void)
{
    RUN_TEST(test_app_fsm_startup_to_ready);
    RUN_TEST(test_app_fsm_normal_cycle_progression);
    RUN_TEST(test_app_fsm_emergency_stop_override);
}
