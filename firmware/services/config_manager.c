/**
 * @file config_manager.c
 * @brief Configuration Manager Implementation
 * @author Sherifred Singh
 */

#include "config_manager.h"
#include "crc16.h"
#include "hal_nvm.h"
#include <string.h>

static config_data_t s_active_config;

static const config_data_t s_factory_defaults = {
    .magic_word               = CONFIG_MAGIC_WORD,
    .schema_version           = CONFIG_SCHEMA_VERSION,
    .payload_len              = sizeof(config_data_t) - sizeof(uint16_t),
    .pre_purge_duration_s     = 30U,
    .disinfect_duration_s     = 300U,
    .dwell_soak_duration_s    = 120U,
    .post_purge_duration_s    = 60U,
    .target_concentration_ppm = 50U,
    .max_temperature_c        = 45U,
    .ambient_leak_limit_ppm   = 5U,
    .min_humidity_rh          = 40U,
    .modbus_slave_id          = 1U,
    .modbus_baud_idx          = 1U,
    .door_interlock_enable    = 1U,
    .auto_purge_on_fault      = 1U,
    .crc16                    = 0U
};

static uint16_t calculate_config_crc(const config_data_t *cfg)
{
    size_t len_to_crc = sizeof(config_data_t) - sizeof(uint16_t);
    return crc16_ccitt((const uint8_t *)cfg, len_to_crc);
}

bool config_manager_validate(const config_data_t *cfg)
{
    if (cfg == NULL) {
        return false;
    }

    if (cfg->magic_word != CONFIG_MAGIC_WORD || cfg->schema_version != CONFIG_SCHEMA_VERSION) {
        return false;
    }

    if (cfg->pre_purge_duration_s < 5U || cfg->pre_purge_duration_s > 600U) return false;
    if (cfg->disinfect_duration_s < 10U || cfg->disinfect_duration_s > 7200U) return false;
    if (cfg->dwell_soak_duration_s > 3600U) return false;
    if (cfg->post_purge_duration_s < 10U || cfg->post_purge_duration_s > 1800U) return false;
    if (cfg->max_temperature_c < 20U || cfg->max_temperature_c > 85U) return false;
    if (cfg->ambient_leak_limit_ppm == 0 || cfg->ambient_leak_limit_ppm > 100U) return false;
    if (cfg->modbus_slave_id == 0 || cfg->modbus_slave_id > 247U) return false;
    if (cfg->modbus_baud_idx > 3U) return false;

    return true;
}

bool config_manager_init(void)
{
    hal_nvm_init();

    config_data_t loaded_cfg;
    if (hal_nvm_read(0, (uint8_t *)&loaded_cfg, sizeof(config_data_t))) {
        uint16_t computed_crc = calculate_config_crc(&loaded_cfg);
        if (computed_crc == loaded_cfg.crc16 && config_manager_validate(&loaded_cfg)) {
            memcpy(&s_active_config, &loaded_cfg, sizeof(config_data_t));
            return true;
        }
    }

    config_manager_restore_defaults();
    return false;
}

const config_data_t *config_manager_get(void)
{
    return &s_active_config;
}

bool config_manager_save(const config_data_t *new_cfg)
{
    if (!config_manager_validate(new_cfg)) {
        return false;
    }

    config_data_t to_write;
    memcpy(&to_write, new_cfg, sizeof(config_data_t));
    to_write.magic_word     = CONFIG_MAGIC_WORD;
    to_write.schema_version = CONFIG_SCHEMA_VERSION;
    to_write.payload_len    = sizeof(config_data_t) - sizeof(uint16_t);
    to_write.crc16          = calculate_config_crc(&to_write);

    if (hal_nvm_write(0, (const uint8_t *)&to_write, sizeof(config_data_t))) {
        memcpy(&s_active_config, &to_write, sizeof(config_data_t));
        return true;
    }

    return false;
}

void config_manager_restore_defaults(void)
{
    memcpy(&s_active_config, &s_factory_defaults, sizeof(config_data_t));
    s_active_config.crc16 = calculate_config_crc(&s_active_config);
    hal_nvm_write(0, (const uint8_t *)&s_active_config, sizeof(config_data_t));
}
