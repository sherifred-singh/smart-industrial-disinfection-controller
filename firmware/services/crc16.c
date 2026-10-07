/**
 * @file crc16.c
 * @brief CRC-16 Calculation Implementation (Table-Accelerated & Bitwise)
 * @author Sherifred Singh
 */

#include "crc16.h"

/* Fast Table-driven CRC-16-CCITT (Poly 0x1021) */
static const uint16_t s_crc16_ccitt_table[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
};

uint16_t crc16_ccitt(const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0) {
        return 0xFFFFU;
    }

    uint16_t crc = 0xFFFFU;

    for (size_t i = 0; i < length; ++i) {
        uint8_t byte = data[i];
        /* Process high nibble */
        uint8_t high_nibble = (uint8_t)((crc >> 12) ^ (byte >> 4));
        crc = (uint16_t)((crc << 4) ^ s_crc16_ccitt_table[high_nibble & 0x0F]);

        /* Process low nibble */
        uint8_t low_nibble = (uint8_t)((crc >> 12) ^ (byte & 0x0F));
        crc = (uint16_t)((crc << 4) ^ s_crc16_ccitt_table[low_nibble & 0x0F]);
    }

    return crc;
}

uint16_t crc16_modbus(const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0) {
        return 0xFFFFU;
    }

    uint16_t crc = 0xFFFFU;

    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            if (crc & 0x0001U) {
                crc = (crc >> 1) ^ 0xA001U;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}
