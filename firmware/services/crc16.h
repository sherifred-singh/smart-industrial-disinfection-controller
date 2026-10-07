/**
 * @file crc16.h
 * @brief CRC-16 Algorithms for NVM Configuration & Modbus RTU Validation
 * @author Sherifred Singh
 */

#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate CRC-16-CCITT (Poly 0x1021, Init 0xFFFF).
 * Used for NVM Parameter Store integrity validation.
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return 16-bit CRC checksum
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t length);

/**
 * @brief Calculate CRC-16-Modbus (Poly 0xA001, Init 0xFFFF).
 * Used for Modbus RTU serial frame transmission and verification.
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return 16-bit Modbus CRC (Low byte first in standard frame)
 */
uint16_t crc16_modbus(const uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* CRC16_H */
