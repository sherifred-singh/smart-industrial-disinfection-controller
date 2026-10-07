/**
 * @file test_runner.c
 * @brief Master Test Suite Runner for Smart Industrial Disinfection Controller
 * @author Sherifred Singh
 */

#include "unity/unity.h"
#include <stdio.h>

/* Forward declare all test suite runners */
extern void run_test_ring_buffer(void);
extern void run_test_crc16(void);
extern void run_test_filter_service(void);
extern void run_test_config_manager(void);
extern void run_test_alarm_fault(void);
extern void run_test_modbus_rtu(void);
extern void run_test_app_fsm(void);
extern void run_test_sensor_to_fsm_integration(void);
extern void run_test_modbus_to_config_integration(void);
extern void run_test_full_cycle(void);

int main(void)
{
    printf("\n");
    printf("==============================================================\n");
    printf("  SMART INDUSTRIAL DISINFECTION CONTROLLER FIRMWARE TEST SUITE\n");
    printf("  Author: Sherifred Singh | Embedded Firmware Architecture\n");
    printf("==============================================================\n");

    int failures = 0;

    unity_begin("01. SPSC Ring Buffer Unit Tests");
    run_test_ring_buffer();
    failures += unity_end();

    unity_begin("02. CRC-16 Checksum Unit Tests (CCITT & Modbus)");
    run_test_crc16();
    failures += unity_end();

    unity_begin("03. Digital Filter Service (EMA & Median-3)");
    run_test_filter_service();
    failures += unity_end();

    unity_begin("04. NVM Parameter Store & Corruption Recovery");
    run_test_config_manager();
    failures += unity_end();

    unity_begin("05. Alarm & Safety Fault Manager");
    run_test_alarm_fault();
    failures += unity_end();

    unity_begin("06. Modbus RTU Slave Protocol Stack");
    run_test_modbus_rtu();
    failures += unity_end();

    unity_begin("07. Application State Machine (FSM) Engine");
    run_test_app_fsm();
    failures += unity_end();

    unity_begin("08. Sensor-to-FSM Integration Tests (Safety Trip)");
    run_test_sensor_to_fsm_integration();
    failures += unity_end();

    unity_begin("09. Modbus-to-Config Integration Tests (Batch Write)");
    run_test_modbus_to_config_integration();
    failures += unity_end();

    unity_begin("10. Full Process Sequence System Test (End-to-End)");
    run_test_full_cycle();
    failures += unity_end();

    printf("==============================================================\n");
    if (failures == 0) {
        printf("  >>> ALL TEST SUITES PASSED CLEANLY WITH ZERO FAILURES! <<<\n");
    } else {
        printf("  >>> FAILED: %d SUITE(S) RECORDED TEST FAILURES! <<<\n", failures);
    }
    printf("==============================================================\n\n");

    return (failures == 0) ? 0 : 1;
}
