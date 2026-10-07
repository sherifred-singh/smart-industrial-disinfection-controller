/**
 * @file nvm_driver.h
 * @brief Non-Volatile Memory Sector Driver
 * @author Sherifred Singh
 */

#ifndef NVM_DRIVER_H
#define NVM_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void nvm_driver_init(void);
bool nvm_driver_read_block(uint32_t offset, uint8_t *dest, size_t len);
bool nvm_driver_write_block(uint32_t offset, const uint8_t *src, size_t len);
bool nvm_driver_format(void);

#ifdef __cplusplus
}
#endif

#endif /* NVM_DRIVER_H */
