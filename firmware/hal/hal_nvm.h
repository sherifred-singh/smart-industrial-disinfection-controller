/**
 * @file hal_nvm.h
 * @brief Hardware Abstraction Layer for Non-Volatile Memory (EEPROM / Flash)
 * @author Sherifred Singh
 * 
 * Hardware-agnostic interface for reading, writing, and erasing non-volatile
 * parameter blocks.
 */

#ifndef HAL_NVM_H
#define HAL_NVM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_NVM_SECTOR_SIZE     1024U

/**
 * @brief Initialize the NVM peripheral (Internal Flash emulated EEPROM / External I2C EEPROM).
 */
void hal_nvm_init(void);

/**
 * @brief Read bytes from non-volatile storage.
 * @param offset Byte offset from base storage address
 * @param buffer Destination buffer
 * @param length Number of bytes to read
 * @return true on success, false on boundary or hardware error
 */
bool hal_nvm_read(uint32_t offset, uint8_t *buffer, size_t length);

/**
 * @brief Write bytes to non-volatile storage.
 * @param offset Byte offset from base storage address
 * @param buffer Source data buffer
 * @param length Number of bytes to write
 * @return true on success, false on boundary or write failure
 */
bool hal_nvm_write(uint32_t offset, const uint8_t *buffer, size_t length);

/**
 * @brief Erase the entire configuration sector.
 * @return true on success, false otherwise
 */
bool hal_nvm_erase(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVM_H */
