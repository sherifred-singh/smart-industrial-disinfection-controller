/**
 * @file modbus_rtu_slave.h
 * @brief Industrial Modbus RTU Slave Communication Stack
 * @author Sherifred Singh
 * 
 * Implements standard Modbus RTU protocol for PLC/SCADA integration over RS-485.
 * Supported Function Codes:
 *   - 0x01: Read Coils
 *   - 0x02: Read Discrete Inputs
 *   - 0x03: Read Holding Registers (Configuration Setpoints)
 *   - 0x04: Read Input Registers (Live Telemetry & Diagnostics)
 *   - 0x05: Write Single Coil (Commands: Start, Stop, Reset)
 *   - 0x06: Write Single Holding Register
 *   - 0x10: Write Multiple Holding Registers
 */

#ifndef MODBUS_RTU_SLAVE_H
#define MODBUS_RTU_SLAVE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus Standard Exception Codes */
#define MODBUS_EX_ILLEGAL_FUNCTION      0x01U
#define MODBUS_EX_ILLEGAL_DATA_ADDRESS  0x02U
#define MODBUS_EX_ILLEGAL_DATA_VALUE    0x03U
#define MODBUS_EX_SLAVE_DEVICE_FAILURE  0x04U

/* Modbus Register Maps */
/* Discrete Inputs (10001+) */
#define MODBUS_DI_ESTOP_NORMAL          0x0000U /**< 1 = Normal, 0 = Tripped */
#define MODBUS_DI_DOOR_CLOSED           0x0001U /**< 1 = Closed, 0 = Open */
#define MODBUS_DI_AIRFLOW_OK            0x0002U /**< 1 = Exhaust flow healthy */
#define MODBUS_DI_SERVICE_KEY           0x0003U /**< 1 = Service mode active */
#define MODBUS_DI_COUNT                 4U

/* Coils (00001+) */
#define MODBUS_COIL_DISINFECT_RELAY     0x0000U /**< Disinfection relay state */
#define MODBUS_COIL_EXHAUST_FAN         0x0001U /**< Exhaust fan state */
#define MODBUS_COIL_DOOR_LOCK           0x0002U /**< Door lock solenoid */
#define MODBUS_COIL_CMD_START           0x0003U /**< Command: Trigger Cycle Start */
#define MODBUS_COIL_CMD_STOP            0x0004U /**< Command: Trigger Cycle Stop */
#define MODBUS_COIL_CMD_FAULT_RESET     0x0005U /**< Command: Reset Latched Faults */
#define MODBUS_COIL_COUNT               6U

/* Input Registers - Live Telemetry (30001+) */
#define MODBUS_IR_SYSTEM_STATE          0x0000U /**< Current FSM State ID */
#define MODBUS_IR_CHAMBER_TEMP_C10      0x0001U /**< Chamber Temp in 0.1°C (e.g. 254 = 25.4°C) */
#define MODBUS_IR_CHAMBER_HUMIDITY_RH   0x0002U /**< Chamber Humidity in % RH */
#define MODBUS_IR_PROCESS_CONC_PPM      0x0003U /**< Chamber Gas Concentration in PPM */
#define MODBUS_IR_AMBIENT_LEAK_PPM      0x0004U /**< External Leak Sensor PPM */
#define MODBUS_IR_CYCLE_ELAPSED_SEC     0x0005U /**< Current phase elapsed seconds */
#define MODBUS_IR_CYCLE_REMAINING_SEC   0x0006U /**< Current phase remaining seconds */
#define MODBUS_IR_ACTIVE_FAULT_MASK     0x0007U /**< Active fault bitmask */
#define MODBUS_IR_UPTIME_SEC_HIGH       0x0008U /**< System uptime (high word) */
#define MODBUS_IR_UPTIME_SEC_LOW        0x0009U /**< System uptime (low word) */
#define MODBUS_IR_COUNT                 10U

/* Holding Registers - Parameters (40001+) */
#define MODBUS_HR_PRE_PURGE_SEC         0x0000U
#define MODBUS_HR_DISINFECT_SEC         0x0001U
#define MODBUS_HR_DWELL_SOAK_SEC        0x0002U
#define MODBUS_HR_POST_PURGE_SEC        0x0003U
#define MODBUS_HR_TARGET_CONC_PPM       0x0004U
#define MODBUS_HR_MAX_TEMP_C            0x0005U
#define MODBUS_HR_AMBIENT_LEAK_LIMIT    0x0006U
#define MODBUS_HR_MIN_HUMIDITY_RH       0x0007U
#define MODBUS_HR_DOOR_INTERLOCK_EN     0x0008U
#define MODBUS_HR_COUNT                 9U

/**
 * @brief Initialize Modbus RTU Slave engine.
 */
void modbus_rtu_init(void);

/**
 * @brief Push a received raw serial byte into the Modbus RX engine.
 * Typically called from UART RX ISR.
 */
void modbus_rtu_on_byte_received(uint8_t byte);

/**
 * @brief Process Modbus state machine & frame parsing (Call periodically in 1ms/10ms loop).
 */
void modbus_rtu_process(void);

/**
 * @brief Directly process a raw frame buffer and populate response (useful for test harnesses).
 * @param req_buf Pointer to request frame
 * @param req_len Length of request
 * @param resp_buf Pointer to response buffer
 * @param max_resp_len Maximum response buffer size
 * @return Length of response frame in bytes (0 if frame ignored / slave address mismatch)
 */
size_t modbus_rtu_process_frame(const uint8_t *req_buf, size_t req_len, uint8_t *resp_buf, size_t max_resp_len);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_SLAVE_H */
