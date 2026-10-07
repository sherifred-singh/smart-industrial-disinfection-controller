/**
 * @file modbus_rtu_slave.c
 * @brief Industrial Modbus RTU Slave Protocol Implementation
 * @author Sherifred Singh
 */

#include "modbus_rtu_slave.h"
#include "crc16.h"
#include "ring_buffer.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "app_fsm.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include <string.h>

#define MODBUS_MAX_FRAME_SIZE   256U
#define MODBUS_T35_TIMEOUT_MS   5U

static uint8_t       s_rx_raw_buf[MODBUS_MAX_FRAME_SIZE];
static ring_buffer_t s_rx_ring;
static uint8_t       s_frame_buf[MODBUS_MAX_FRAME_SIZE];
static size_t        s_frame_len = 0;
static uint32_t      s_last_byte_time_ms = 0;

static volatile bool s_cmd_start_requested = false;
static volatile bool s_cmd_stop_requested = false;
static volatile bool s_cmd_reset_requested = false;

bool modbus_rtu_get_start_cmd(void)
{
    if (s_cmd_start_requested) {
        s_cmd_start_requested = false;
        return true;
    }
    return false;
}

bool modbus_rtu_get_stop_cmd(void)
{
    if (s_cmd_stop_requested) {
        s_cmd_stop_requested = false;
        return true;
    }
    return false;
}

bool modbus_rtu_get_reset_cmd(void)
{
    if (s_cmd_reset_requested) {
        s_cmd_reset_requested = false;
        return true;
    }
    return false;
}

void modbus_rtu_init(void)
{
    ring_buffer_init(&s_rx_ring, s_rx_raw_buf, MODBUS_MAX_FRAME_SIZE);
    s_frame_len = 0;
    s_last_byte_time_ms = 0;
    s_cmd_start_requested = false;
    s_cmd_stop_requested = false;
    s_cmd_reset_requested = false;

    const config_data_t *cfg = config_manager_get();
    uint32_t bauds[4] = {9600, 19200, 38400, 115200};
    uint32_t baud = (cfg->modbus_baud_idx < 4) ? bauds[cfg->modbus_baud_idx] : 19200;

    hal_uart_init(HAL_UART_PORT_MODBUS, baud, (hal_uart_rx_callback_t)modbus_rtu_on_byte_received);
}

void modbus_rtu_on_byte_received(uint8_t byte)
{
    ring_buffer_push(&s_rx_ring, byte);
    s_last_byte_time_ms = hal_timer_get_ms();
}

static size_t build_exception(uint8_t slave_id, uint8_t func, uint8_t ex_code, uint8_t *resp)
{
    resp[0] = slave_id;
    resp[1] = func | 0x80U;
    resp[2] = ex_code;
    uint16_t crc = crc16_modbus(resp, 3);
    resp[3] = (uint8_t)(crc & 0xFFU);
    resp[4] = (uint8_t)((crc >> 8) & 0xFFU);
    return 5;
}

static bool read_discrete_input(uint16_t addr, bool *val)
{
    switch (addr) {
        case MODBUS_DI_ESTOP_NORMAL:
            *val = (hal_gpio_read(HAL_GPIO_IN_ESTOP_NC) == HAL_GPIO_LEVEL_HIGH);
            return true;
        case MODBUS_DI_DOOR_CLOSED:
            *val = (hal_gpio_read(HAL_GPIO_IN_DOOR_INTERLOCK) == HAL_GPIO_LEVEL_HIGH);
            return true;
        case MODBUS_DI_AIRFLOW_OK:
            *val = (hal_gpio_read(HAL_GPIO_IN_AIRFLOW_SW) == HAL_GPIO_LEVEL_HIGH);
            return true;
        case MODBUS_DI_SERVICE_KEY:
            *val = (hal_gpio_read(HAL_GPIO_IN_SERVICE_KEY) == HAL_GPIO_LEVEL_HIGH);
            return true;
        default:
            return false;
    }
}

static bool read_coil(uint16_t addr, bool *val)
{
    switch (addr) {
        case MODBUS_COIL_DISINFECT_RELAY:
            *val = actuator_driver_is_disinfect_on();
            return true;
        case MODBUS_COIL_EXHAUST_FAN:
            *val = actuator_driver_is_exhaust_on();
            return true;
        case MODBUS_COIL_DOOR_LOCK:
            *val = actuator_driver_is_door_locked();
            return true;
        default:
            return false;
    }
}

static bool read_input_register(uint16_t addr, uint16_t *val)
{
    sensor_data_t sensors;
    sensor_driver_get_readings(&sensors);

    switch (addr) {
        case MODBUS_IR_SYSTEM_STATE:
            *val = (uint16_t)app_fsm_get_state();
            return true;
        case MODBUS_IR_CHAMBER_TEMP_C10:
            *val = (uint16_t)sensors.chamber_temp_c10;
            return true;
        case MODBUS_IR_CHAMBER_HUMIDITY_RH:
            *val = sensors.chamber_humidity_rh;
            return true;
        case MODBUS_IR_PROCESS_CONC_PPM:
            *val = sensors.chamber_conc_ppm;
            return true;
        case MODBUS_IR_AMBIENT_LEAK_PPM:
            *val = sensors.ambient_leak_ppm;
            return true;
        case MODBUS_IR_CYCLE_ELAPSED_SEC:
            *val = (uint16_t)app_fsm_get_phase_elapsed_sec();
            return true;
        case MODBUS_IR_CYCLE_REMAINING_SEC:
            *val = (uint16_t)app_fsm_get_phase_remaining_sec();
            return true;
        case MODBUS_IR_ACTIVE_FAULT_MASK:
            *val = (uint16_t)(alarm_fault_get_active_mask() & 0xFFFFU);
            return true;
        case MODBUS_IR_UPTIME_SEC_HIGH:
            *val = (uint16_t)((hal_timer_get_ms() / 1000U) >> 16);
            return true;
        case MODBUS_IR_UPTIME_SEC_LOW:
            *val = (uint16_t)((hal_timer_get_ms() / 1000U) & 0xFFFFU);
            return true;
        default:
            return false;
    }
}

static bool read_holding_register(uint16_t addr, uint16_t *val)
{
    const config_data_t *cfg = config_manager_get();
    switch (addr) {
        case MODBUS_HR_PRE_PURGE_SEC:      *val = cfg->pre_purge_duration_s; return true;
        case MODBUS_HR_DISINFECT_SEC:      *val = cfg->disinfect_duration_s; return true;
        case MODBUS_HR_DWELL_SOAK_SEC:     *val = cfg->dwell_soak_duration_s; return true;
        case MODBUS_HR_POST_PURGE_SEC:     *val = cfg->post_purge_duration_s; return true;
        case MODBUS_HR_TARGET_CONC_PPM:    *val = cfg->target_concentration_ppm; return true;
        case MODBUS_HR_MAX_TEMP_C:         *val = cfg->max_temperature_c; return true;
        case MODBUS_HR_AMBIENT_LEAK_LIMIT: *val = cfg->ambient_leak_limit_ppm; return true;
        case MODBUS_HR_MIN_HUMIDITY_RH:    *val = cfg->min_humidity_rh; return true;
        case MODBUS_HR_DOOR_INTERLOCK_EN:  *val = cfg->door_interlock_enable; return true;
        default: return false;
    }
}

static bool write_holding_register(uint16_t addr, uint16_t val)
{
    config_data_t cfg = *config_manager_get();
    switch (addr) {
        case MODBUS_HR_PRE_PURGE_SEC:      cfg.pre_purge_duration_s = val; break;
        case MODBUS_HR_DISINFECT_SEC:      cfg.disinfect_duration_s = val; break;
        case MODBUS_HR_DWELL_SOAK_SEC:     cfg.dwell_soak_duration_s = val; break;
        case MODBUS_HR_POST_PURGE_SEC:     cfg.post_purge_duration_s = val; break;
        case MODBUS_HR_TARGET_CONC_PPM:    cfg.target_concentration_ppm = val; break;
        case MODBUS_HR_MAX_TEMP_C:         cfg.max_temperature_c = val; break;
        case MODBUS_HR_AMBIENT_LEAK_LIMIT: cfg.ambient_leak_limit_ppm = val; break;
        case MODBUS_HR_MIN_HUMIDITY_RH:    cfg.min_humidity_rh = val; break;
        case MODBUS_HR_DOOR_INTERLOCK_EN:  cfg.door_interlock_enable = (uint8_t)val; break;
        default: return false;
    }
    return config_manager_save(&cfg);
}

size_t modbus_rtu_process_frame(const uint8_t *req, size_t req_len, uint8_t *resp, size_t max_resp)
{
    if (req == NULL || resp == NULL || req_len < 4) {
        return 0;
    }

    uint16_t received_crc = (uint16_t)req[req_len - 2] | ((uint16_t)req[req_len - 1] << 8);
    uint16_t calculated_crc = crc16_modbus(req, req_len - 2);
    if (received_crc != calculated_crc) {
        return 0;
    }

    uint8_t slave_id = req[0];
    const config_data_t *cfg = config_manager_get();

    if (slave_id != cfg->modbus_slave_id && slave_id != 0) {
        return 0;
    }

    uint8_t func = req[1];

    if (func == 0x01 || func == 0x02) {
        uint16_t start_addr = ((uint16_t)req[2] << 8) | req[3];
        uint16_t count      = ((uint16_t)req[4] << 8) | req[5];

        if (count == 0 || count > 32) {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
        }

        uint8_t byte_count = (uint8_t)((count + 7) / 8);
        if (((size_t)(3 + byte_count + 2)) > max_resp) {
            return build_exception(slave_id, func, MODBUS_EX_SLAVE_DEVICE_FAILURE, resp);
        }

        resp[0] = slave_id;
        resp[1] = func;
        resp[2] = byte_count;
        memset(&resp[3], 0, byte_count);

        for (uint16_t i = 0; i < count; ++i) {
            bool val = false;
            bool ok = (func == 0x01) ? read_coil(start_addr + i, &val) : read_discrete_input(start_addr + i, &val);
            if (!ok) {
                return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_ADDRESS, resp);
            }
            if (val) {
                resp[3 + (i / 8)] |= (1U << (i % 8));
            }
        }

        uint16_t crc = crc16_modbus(resp, 3 + byte_count);
        resp[3 + byte_count] = (uint8_t)(crc & 0xFFU);
        resp[4 + byte_count] = (uint8_t)((crc >> 8) & 0xFFU);
        return (5 + byte_count);
    }

    else if (func == 0x03 || func == 0x04) {
        uint16_t start_addr = ((uint16_t)req[2] << 8) | req[3];
        uint16_t count      = ((uint16_t)req[4] << 8) | req[5];

        if (count == 0 || count > 32) {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
        }

        uint8_t byte_count = (uint8_t)(count * 2);
        if (((size_t)(3 + byte_count + 2)) > max_resp) {
            return build_exception(slave_id, func, MODBUS_EX_SLAVE_DEVICE_FAILURE, resp);
        }

        resp[0] = slave_id;
        resp[1] = func;
        resp[2] = byte_count;

        for (uint16_t i = 0; i < count; ++i) {
            uint16_t val = 0;
            bool ok = (func == 0x03) ? read_holding_register(start_addr + i, &val) : read_input_register(start_addr + i, &val);
            if (!ok) {
                return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_ADDRESS, resp);
            }
            resp[3 + (i * 2)]     = (uint8_t)((val >> 8) & 0xFFU);
            resp[3 + (i * 2) + 1] = (uint8_t)(val & 0xFFU);
        }

        uint16_t crc = crc16_modbus(resp, 3 + byte_count);
        resp[3 + byte_count]     = (uint8_t)(crc & 0xFFU);
        resp[3 + byte_count + 1] = (uint8_t)((crc >> 8) & 0xFFU);
        return (5 + byte_count);
    }

    else if (func == 0x05) {
        uint16_t coil_addr = ((uint16_t)req[2] << 8) | req[3];
        uint16_t coil_val  = ((uint16_t)req[4] << 8) | req[5];

        if (coil_val != 0xFF00U && coil_val != 0x0000U) {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
        }

        if (coil_addr == MODBUS_COIL_CMD_START && coil_val == 0xFF00U) {
            s_cmd_start_requested = true;
        } else if (coil_addr == MODBUS_COIL_CMD_STOP && coil_val == 0xFF00U) {
            s_cmd_stop_requested = true;
        } else if (coil_addr == MODBUS_COIL_CMD_FAULT_RESET && coil_val == 0xFF00U) {
            s_cmd_reset_requested = true;
        } else {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_ADDRESS, resp);
        }

        memcpy(resp, req, req_len);
        return req_len;
    }

    else if (func == 0x06) {
        uint16_t reg_addr = ((uint16_t)req[2] << 8) | req[3];
        uint16_t reg_val  = ((uint16_t)req[4] << 8) | req[5];

        if (!write_holding_register(reg_addr, reg_val)) {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
        }

        memcpy(resp, req, req_len);
        return req_len;
    }

    else if (func == 0x10) {
        uint16_t start_addr = ((uint16_t)req[2] << 8) | req[3];
        uint16_t count      = ((uint16_t)req[4] << 8) | req[5];
        uint8_t  byte_count = req[6];

        if (count == 0 || (count * 2) != byte_count) {
            return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
        }

        for (uint16_t i = 0; i < count; ++i) {
            uint16_t val = ((uint16_t)req[7 + (i * 2)] << 8) | req[7 + (i * 2) + 1];
            if (!write_holding_register(start_addr + i, val)) {
                return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_DATA_VALUE, resp);
            }
        }

        resp[0] = slave_id;
        resp[1] = func;
        resp[2] = req[2];
        resp[3] = req[3];
        resp[4] = req[4];
        resp[5] = req[5];
        uint16_t crc = crc16_modbus(resp, 6);
        resp[6] = (uint8_t)(crc & 0xFFU);
        resp[7] = (uint8_t)((crc >> 8) & 0xFFU);
        return 8;
    }

    return build_exception(slave_id, func, MODBUS_EX_ILLEGAL_FUNCTION, resp);
}

void modbus_rtu_process(void)
{
    uint8_t byte = 0;
    while (ring_buffer_pop(&s_rx_ring, &byte)) {
        if (s_frame_len < MODBUS_MAX_FRAME_SIZE) {
            s_frame_buf[s_frame_len++] = byte;
        }
    }

    if (s_frame_len > 0) {
        if (hal_timer_has_elapsed(s_last_byte_time_ms, MODBUS_T35_TIMEOUT_MS)) {
            uint8_t resp_buf[MODBUS_MAX_FRAME_SIZE];
            size_t resp_len = modbus_rtu_process_frame(s_frame_buf, s_frame_len, resp_buf, sizeof(resp_buf));

            if (resp_len > 0) {
                hal_uart_write(HAL_UART_PORT_MODBUS, resp_buf, resp_len);
            }

            s_frame_len = 0;
        }
    }
}
