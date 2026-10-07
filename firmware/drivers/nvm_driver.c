/**
 * @file nvm_driver.c
 * @brief Non-Volatile Memory Sector Driver Implementation
 * @author Sherifred Singh
 */

#include "nvm_driver.h"
#include "hal_nvm.h"

void nvm_driver_init(void)
{
    hal_nvm_init();
}

bool nvm_driver_read_block(uint32_t offset, uint8_t *dest, size_t len)
{
    return hal_nvm_read(offset, dest, len);
}

bool nvm_driver_write_block(uint32_t offset, const uint8_t *src, size_t len)
{
    return hal_nvm_write(offset, src, len);
}

bool nvm_driver_format(void)
{
    return hal_nvm_erase();
}
