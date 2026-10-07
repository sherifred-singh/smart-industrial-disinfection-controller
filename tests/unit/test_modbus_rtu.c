/**
 * @file test_modbus_rtu.c
 * @brief Unit Tests for Modbus RTU Slave Engine
 * @author Sherifred Singh
 */

#include "unity.h"
#include "modbus_rtu_slave.h"
#include "crc16.h"
#include "config_manager.h"

void test_modbus_rtu_read_holding_registers(void)
{
    config_manager_init();

    uint8_t req[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00};
    uint16_t req_crc = crc16_modbus(req, 6);
    req[6] = (uint8_t)(req_crc & 0xFFU);
    req[7] = (uint8_t)((req_crc >> 8) & 0xFFU);

    uint8_t resp[32];
    size_t resp_len = modbus_rtu_process_frame(req, sizeof(req), resp, sizeof(resp));

    TEST_ASSERT_EQUAL_UINT(13, resp_len);
    TEST_ASSERT_EQUAL_UINT(0x01, resp[0]);
    TEST_ASSERT_EQUAL_UINT(0x03, resp[1]);
    TEST_ASSERT_EQUAL_UINT(8, resp[2]);

    uint16_t reg0 = ((uint16_t)resp[3] << 8) | resp[4];
    TEST_ASSERT_EQUAL_UINT(30, reg0);
}

void test_modbus_rtu_write_single_holding_register(void)
{
    uint8_t req[8] = {0x01, 0x06, 0x00, 0x01, 0x02, 0x58, 0x00, 0x00};
    uint16_t req_crc = crc16_modbus(req, 6);
    req[6] = (uint8_t)(req_crc & 0xFFU);
    req[7] = (uint8_t)((req_crc >> 8) & 0xFFU);

    uint8_t resp[32];
    size_t resp_len = modbus_rtu_process_frame(req, sizeof(req), resp, sizeof(resp));

    TEST_ASSERT_EQUAL_UINT(8, resp_len);

    const config_data_t *cfg = config_manager_get();
    TEST_ASSERT_EQUAL_UINT(600, cfg->disinfect_duration_s);
}

void test_modbus_rtu_crc_error_rejection(void)
{
    uint8_t req[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0xFF, 0xFF};
    uint8_t resp[32];
    size_t resp_len = modbus_rtu_process_frame(req, sizeof(req), resp, sizeof(resp));

    TEST_ASSERT_EQUAL_UINT(0, resp_len);
}

void run_test_modbus_rtu(void)
{
    RUN_TEST(test_modbus_rtu_read_holding_registers);
    RUN_TEST(test_modbus_rtu_write_single_holding_register);
    RUN_TEST(test_modbus_rtu_crc_error_rejection);
}
