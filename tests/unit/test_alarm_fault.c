/**
 * @file test_alarm_fault.c
 * @brief Unit Tests for Alarm & Fault Management
 * @author Sherifred Singh
 */

#include "unity.h"
#include "alarm_fault_manager.h"
#include "hal_gpio.h"

extern void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level);

void test_alarm_fault_set_and_severity(void)
{
    alarm_fault_init();
    TEST_ASSERT_FALSE(alarm_fault_has_critical());
    TEST_ASSERT_FALSE(alarm_fault_has_warning());

    alarm_fault_set(FAULT_CODE_COMM_TIMEOUT, FAULT_SEV_WARNING);
    TEST_ASSERT_TRUE(alarm_fault_has_warning());
    TEST_ASSERT_FALSE(alarm_fault_has_critical());
    TEST_ASSERT_EQUAL_INT(FAULT_SEV_WARNING, alarm_fault_get_highest_severity());

    alarm_fault_set(FAULT_CODE_DOOR_BREACH, FAULT_SEV_CRITICAL);
    TEST_ASSERT_TRUE(alarm_fault_has_critical());
    TEST_ASSERT_EQUAL_INT(FAULT_SEV_CRITICAL, alarm_fault_get_highest_severity());
}

void test_alarm_fault_reset_latch(void)
{
    alarm_fault_init();
    hal_gpio_sim_set_input(HAL_GPIO_IN_ESTOP_NC, HAL_GPIO_LEVEL_LOW);

    alarm_fault_set(FAULT_CODE_ESTOP_ACTIVE, FAULT_SEV_CRITICAL);
    TEST_ASSERT_TRUE(alarm_fault_has_critical());

    TEST_ASSERT_FALSE(alarm_fault_reset_all());
    TEST_ASSERT_TRUE(alarm_fault_has_critical());

    hal_gpio_sim_set_input(HAL_GPIO_IN_ESTOP_NC, HAL_GPIO_LEVEL_HIGH);

    TEST_ASSERT_TRUE(alarm_fault_reset_all());
    TEST_ASSERT_FALSE(alarm_fault_has_critical());
}

void test_alarm_fault_interlock_tamper_detection(void)
{
    alarm_fault_init();

    /* Simulate intermittent door sensor breach bounce */
    hal_gpio_sim_set_input(HAL_GPIO_IN_DOOR_INTERLOCK, HAL_GPIO_LEVEL_LOW);
    alarm_fault_set(FAULT_CODE_DOOR_BREACH, FAULT_SEV_CRITICAL);

    TEST_ASSERT_TRUE(alarm_fault_has_critical());
    TEST_ASSERT_EQUAL_INT(FAULT_SEV_CRITICAL, alarm_fault_get_highest_severity());

    /* Even if physical sensor momentarily bounces back high, critical fault must remain latched */
    hal_gpio_sim_set_input(HAL_GPIO_IN_DOOR_INTERLOCK, HAL_GPIO_LEVEL_HIGH);
    TEST_ASSERT_TRUE(alarm_fault_has_critical());

    /* Reset must cleanly clear the fault once verified secure */
    TEST_ASSERT_TRUE(alarm_fault_reset_all());
    TEST_ASSERT_FALSE(alarm_fault_has_critical());
}

void run_test_alarm_fault(void)
{
    RUN_TEST(test_alarm_fault_set_and_severity);
    RUN_TEST(test_alarm_fault_reset_latch);
    RUN_TEST(test_alarm_fault_interlock_tamper_detection);
}
