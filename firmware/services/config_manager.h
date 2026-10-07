/**
 * @file config_manager.h
 * @brief Non-Volatile Parameter Configuration Manager with CRC-16 Validation
 * @author Sherifred Singh
 * 
 * Manages industrial process setpoints, timing profiles, sensor thresholds,
 * and communication settings with integrity verification and safe fallback.
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_MAGIC_WORD       0x53494443U /* "SIDC" */
#define CONFIG_SCHEMA_VERSION   0x0100U     /* v1.0 */

/**
 * @brief Industrial Disinfection & Environment Configuration Parameters
 */
typedef struct {
    /* Header & Integrity */
    uint32_t magic_word;            /**< Must match CONFIG_MAGIC_WORD */
    uint16_t schema_version;        /**< Format version */
    uint16_t payload_len;           /**< Byte count of parameters */

    /* Process Timing (Seconds) */
    uint16_t pre_purge_duration_s;   /**< Pre-cycle exhaust purge (e.g. 10 - 300s) */
    uint16_t disinfect_duration_s;   /**< Active generation/dispersal duration (e.g. 30 - 3600s) */
    uint16_t dwell_soak_duration_s;  /**< Chemical soak/contact time (e.g. 10 - 1800s) */
    uint16_t post_purge_duration_s;  /**< Post-cycle scrubber ventilation (e.g. 30 - 600s) */

    /* Process & Safety Thresholds */
    uint16_t target_concentration_ppm;/**< Chamber target concentration setpoint */
    uint16_t max_temperature_c;       /**< Maximum allowable chamber temp in °C (e.g. 45°C) */
    uint16_t ambient_leak_limit_ppm;  /**< Safety trip threshold for external ambient leak */
    uint16_t min_humidity_rh;         /**< Minimum relative humidity % for process efficacy */

    /* Communication Settings */
    uint8_t  modbus_slave_id;         /**< Modbus RTU Address (1 - 247) */
    uint8_t  modbus_baud_idx;         /**< 0=9600, 1=19200, 2=38400, 3=115200 */
    uint8_t  door_interlock_enable;   /**< 1 = Enforce door switch interlock, 0 = Bypassed (test mode) */
    uint8_t  auto_purge_on_fault;     /**< 1 = Automatically run scrubber fan on emergency trip */

    /* Checksum */
    uint16_t crc16;                   /**< CRC-16-CCITT computed over entire structure except crc field */
} __attribute__((packed)) config_data_t;

/**
 * @brief Initialize the configuration manager, read NVM, and validate CRC.
 * If NVM is uninitialized or corrupted, auto-loads factory defaults and saves.
 * @return true if valid configuration was loaded from NVM, false if restored to defaults.
 */
bool config_manager_init(void);

/**
 * @brief Get a read-only pointer to the active active configuration.
 */
const config_data_t *config_manager_get(void);

/**
 * @brief Update and save a new configuration to NVM with CRC computation.
 * @param new_cfg Pointer to updated configuration struct
 * @return true on valid parameters and successful NVM write, false otherwise.
 */
bool config_manager_save(const config_data_t *new_cfg);

/**
 * @brief Reset active configuration to factory default safe values and persist to NVM.
 */
void config_manager_restore_defaults(void);

/**
 * @brief Validate parameter ranges within safety bounds.
 * @param cfg Pointer to config struct
 * @return true if all values fall within physical safety limits.
 */
bool config_manager_validate(const config_data_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_MANAGER_H */
