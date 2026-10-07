/**
 * @file hal_uart_stm32.c
 * @brief STM32F4xx Hardware Implementation for UART & RS-485
 * @author Sherifred Singh
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_uart.h"
#include "../../hal/hal_gpio.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2; /* Debug Console on USART2 (PA2/PA3) */
extern UART_HandleTypeDef huart1; /* Modbus RS-485 on USART1 (PA9/PA10) */

static hal_uart_rx_callback_t s_rx_callbacks[HAL_UART_PORT_COUNT] = {NULL, NULL};
static uint8_t s_rx_byte_buf[HAL_UART_PORT_COUNT];

void hal_uart_init(hal_uart_port_t port, uint32_t baud_rate, hal_uart_rx_callback_t rx_cb)
{
    (void)baud_rate;
    if (port < HAL_UART_PORT_COUNT) {
        s_rx_callbacks[port] = rx_cb;
        if (port == HAL_UART_PORT_DEBUG) {
            HAL_UART_Receive_IT(&huart2, &s_rx_byte_buf[port], 1);
        } else if (port == HAL_UART_PORT_MODBUS) {
            HAL_UART_Receive_IT(&huart1, &s_rx_byte_buf[port], 1);
        }
    }
}

size_t hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0) {
        return 0;
    }

    if (port == HAL_UART_PORT_DEBUG) {
        HAL_UART_Transmit(&huart2, (uint8_t *)data, (uint16_t)length, 100);
        return length;
    } else if (port == HAL_UART_PORT_MODBUS) {
        /* Enable RS-485 Driver Transmit Mode (DE High) */
        hal_gpio_write(HAL_GPIO_OUT_RS485_DE, HAL_GPIO_LEVEL_HIGH);
        HAL_UART_Transmit(&huart1, (uint8_t *)data, (uint16_t)length, 200);
        /* Wait for TC (Transmission Complete) flag */
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
        /* Return RS-485 Transceiver to Receive Mode (DE Low) */
        hal_gpio_write(HAL_GPIO_OUT_RS485_DE, HAL_GPIO_LEVEL_LOW);
        return length;
    }
    return 0;
}

bool hal_uart_putc(hal_uart_port_t port, uint8_t byte)
{
    return (hal_uart_write(port, &byte, 1) == 1);
}

bool hal_uart_getc(hal_uart_port_t port, uint8_t *byte)
{
    (void)port;
    (void)byte;
    return false; /* In STM32 port, incoming stream is processed via ISR callbacks */
}

bool hal_uart_tx_complete(hal_uart_port_t port)
{
    if (port == HAL_UART_PORT_MODBUS) {
        return (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != RESET);
    }
    return true;
}

/* STM32 HAL RX Interrupt Handler */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2) {
        if (s_rx_callbacks[HAL_UART_PORT_DEBUG]) {
            s_rx_callbacks[HAL_UART_PORT_DEBUG](HAL_UART_PORT_DEBUG, s_rx_byte_buf[HAL_UART_PORT_DEBUG]);
        }
        HAL_UART_Receive_IT(&huart2, &s_rx_byte_buf[HAL_UART_PORT_DEBUG], 1);
    } else if (huart == &huart1) {
        if (s_rx_callbacks[HAL_UART_PORT_MODBUS]) {
            s_rx_callbacks[HAL_UART_PORT_MODBUS](HAL_UART_PORT_MODBUS, s_rx_byte_buf[HAL_UART_PORT_MODBUS]);
        }
        HAL_UART_Receive_IT(&huart1, &s_rx_byte_buf[HAL_UART_PORT_MODBUS], 1);
    }
}

#endif /* TARGET_STM32F4 */
