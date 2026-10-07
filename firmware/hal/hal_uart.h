/**
 * @file hal_uart.h
 * @brief Hardware Abstraction Layer for Serial UART & RS-485
 * @author Sherifred Singh
 * 
 * Non-blocking UART communication abstraction supporting interrupt-driven or
 * DMA-driven transfers with RS-485 transceiver direction (DE/!RE) control.
 */

#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART Port Identifiers
 */
typedef enum {
    HAL_UART_PORT_DEBUG = 0,    /**< Primary Debug / CLI Console (USB Virtual COM / RS-232) */
    HAL_UART_PORT_MODBUS,       /**< Industrial Modbus RTU (RS-485 Half-Duplex) */
    HAL_UART_PORT_COUNT
} hal_uart_port_t;

/**
 * @brief RX Byte Callback Function Pointer
 */
typedef void (*hal_uart_rx_callback_t)(hal_uart_port_t port, uint8_t byte);

/**
 * @brief Initialize a specific UART port with the given baud rate.
 * @param port Port identifier
 * @param baud_rate Target baud (e.g. 9600, 19200, 115200)
 * @param rx_cb Optional interrupt callback for received bytes
 */
void hal_uart_init(hal_uart_port_t port, uint32_t baud_rate, hal_uart_rx_callback_t rx_cb);

/**
 * @brief Transmit a buffer of bytes over UART (non-blocking / DMA / ISR).
 * @param port Port identifier
 * @param data Pointer to data buffer
 * @param length Number of bytes to transmit
 * @return Number of bytes accepted into transmit buffer
 */
size_t hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t length);

/**
 * @brief Write a single byte over UART.
 * @param port Port identifier
 * @param byte Byte to send
 * @return true if byte was buffered, false if buffer full
 */
bool hal_uart_putc(hal_uart_port_t port, uint8_t byte);

/**
 * @brief Read a single byte from the hardware/FIFO if available.
 * @param port Port identifier
 * @param byte Pointer to store received byte
 * @return true if a byte was read, false if empty
 */
bool hal_uart_getc(hal_uart_port_t port, uint8_t *byte);

/**
 * @brief Check if transmit peripheral is idle (used for RS-485 direction switching).
 * @param port Port identifier
 * @return true if TX shift register is completely empty
 */
bool hal_uart_tx_complete(hal_uart_port_t port);

#ifdef __cplusplus
}
#endif

#endif /* HAL_UART_H */
