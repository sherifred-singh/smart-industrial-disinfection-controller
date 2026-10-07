/**
 * @file hal_nvm_sim.c
 * @brief Host Simulation Implementation for Non-Volatile Memory (RAM/File backed)
 * @author Sherifred Singh
 */

#include "hal_nvm.h"
#include <string.h>

static uint8_t s_nvm_storage[HAL_NVM_SECTOR_SIZE];
static bool s_nvm_initialized = false;

void hal_nvm_init(void)
{
    if (!s_nvm_initialized) {
        memset(s_nvm_storage, 0xFF, sizeof(s_nvm_storage));
        s_nvm_initialized = true;
    }
}

bool hal_nvm_read(uint32_t offset, uint8_t *buffer, size_t length)
{
    if (!s_nvm_initialized) {
        hal_nvm_init();
    }

    if (buffer == NULL || (offset + length) > HAL_NVM_SECTOR_SIZE) {
        return false;
    }

    memcpy(buffer, &s_nvm_storage[offset], length);
    return true;
}

bool hal_nvm_write(uint32_t offset, const uint8_t *buffer, size_t length)
{
    if (!s_nvm_initialized) {
        hal_nvm_init();
    }

    if (buffer == NULL || (offset + length) > HAL_NVM_SECTOR_SIZE) {
        return false;
    }

    memcpy(&s_nvm_storage[offset], buffer, length);
    return true;
}

bool hal_nvm_erase(void)
{
    memset(s_nvm_storage, 0xFF, sizeof(s_nvm_storage));
    return true;
}

void hal_nvm_sim_corrupt_byte(uint32_t offset, uint8_t bad_value)
{
    if (offset < HAL_NVM_SECTOR_SIZE) {
        s_nvm_storage[offset] = bad_value;
    }
}
