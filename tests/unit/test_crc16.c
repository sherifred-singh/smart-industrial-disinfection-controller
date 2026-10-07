/**
 * @file test_crc16.c
 * @brief Unit Tests for CRC-16-CCITT and CRC-16-Modbus Checksums
 * @author Sherifred Singh
 */

#include "unity.h"
#include "crc16.h"

void test_crc16_ccitt_known_vector(void)
{
    const uint8_t test_vector[] = "123456789";
    uint16_t crc = crc16_ccitt(test_vector, 9);
    TEST_ASSERT_EQUAL_UINT(0x29B1, crc);
}

void test_crc16_modbus_known_vector(void)
{
    const uint8_t modbus_req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    uint16_t crc = crc16_modbus(modbus_req, 6);
    TEST_ASSERT_EQUAL_UINT(0xCDC5, crc);
}

void run_test_crc16(void)
{
    RUN_TEST(test_crc16_ccitt_known_vector);
    RUN_TEST(test_crc16_modbus_known_vector);
}
