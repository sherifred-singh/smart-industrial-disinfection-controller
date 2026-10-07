/**
 * @file test_modbus_to_config.c
 * @brief Integration Test: Modbus FC 0x10 Multiple Register Write -> NVM Save & Reboot
 * @author Sherifred Singh
 */

#include "unity.h"
#include "modbus_rtu_slave.h"
#include "config_manager.h"
#include "crc16.h"

void test_integration_modbus_batch_config_update(void)
{
    config_manager_init();

    uint8_t req[15] = {
        0x01, 0x10, 0x00, 0x00, 0x00, 0x03, 0x06,
        0x00, 0x32, 0x02, 0x58, 0x00, 0xB4,
        0x00, 0x00
    };
    uint16_t req_crc = crc16_modbus(req, 13);
    req[13] = (uint8_t)(req_crc & 0xFFU);
    req[14] = (uint8_t)((req_crc >> 8) & 0xFFU);

    uint8_t resp[32];
    size_t resp_len = modbus_rtu_process_frame(req, sizeof(req), resp, sizeof(resp));

    TEST_ASSERT_EQUAL_UINT(8, resp_len);
    TEST_ASSERT_EQUAL_UINT(0x01, resp[0]);
    TEST_ASSERT_EQUAL_UINT(0x10, resp[1]);

    bool reboot_ok = config_manager_init();
    TEST_ASSERT_TRUE(reboot_ok);

    const config_data_t *cfg = config_manager_get();
    TEST_ASSERT_EQUAL_UINT(50, cfg->pre_purge_duration_s);
    TEST_ASSERT_EQUAL_UINT(600, cfg->disinfect_duration_s);
    TEST_ASSERT_EQUAL_UINT(180, cfg->dwell_soak_duration_s);
}

void run_test_modbus_to_config_integration(void)
{
    RUN_TEST(test_integration_modbus_batch_config_update);
}
