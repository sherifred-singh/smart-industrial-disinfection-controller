/**
 * @file hal_nvm_stm32.c
 * @brief STM32F4xx Hardware Implementation for NVM (Internal Flash Emulated EEPROM)
 * @author Sherifred Singh
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_nvm.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/* Use Sector 7 of STM32F401 (128 KB from 0x08060000) for configuration storage */
#define STM32_FLASH_NVM_BASE_ADDR   0x08060000U
#define STM32_FLASH_NVM_SECTOR      FLASH_SECTOR_7

void hal_nvm_init(void)
{
    /* Flash is natively readable */
}

bool hal_nvm_read(uint32_t offset, uint8_t *buffer, size_t length)
{
    if (buffer == NULL || (offset + length) > HAL_NVM_SECTOR_SIZE) {
        return false;
    }

    const uint8_t *flash_ptr = (const uint8_t *)(STM32_FLASH_NVM_BASE_ADDR + offset);
    memcpy(buffer, flash_ptr, length);
    return true;
}

bool hal_nvm_write(uint32_t offset, const uint8_t *buffer, size_t length)
{
    if (buffer == NULL || (offset + length) > HAL_NVM_SECTOR_SIZE) {
        return false;
    }

    /* Buffer current sector in RAM, update, erase, and write */
    uint8_t sector_cache[HAL_NVM_SECTOR_SIZE];
    hal_nvm_read(0, sector_cache, HAL_NVM_SECTOR_SIZE);
    memcpy(&sector_cache[offset], buffer, length);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = STM32_FLASH_NVM_SECTOR;
    erase.NbSectors    = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t sector_error = 0;
    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    for (size_t i = 0; i < HAL_NVM_SECTOR_SIZE; i += 4) {
        uint32_t word = 0;
        memcpy(&word, &sector_cache[i], 4);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, STM32_FLASH_NVM_BASE_ADDR + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    HAL_FLASH_Lock();
    return true;
}

bool hal_nvm_erase(void)
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = STM32_FLASH_NVM_SECTOR;
    erase.NbSectors    = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t sector_error = 0;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase, &sector_error);
    HAL_FLASH_Lock();
    return (status == HAL_OK);
}

#endif /* TARGET_STM32F4 */
