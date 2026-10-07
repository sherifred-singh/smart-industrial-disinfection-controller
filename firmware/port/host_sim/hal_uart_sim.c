/**
 * @file hal_uart_sim.c
 * @brief Host Simulation Implementation for Serial UART & RS-485
 * @author Sherifred Singh
 */

#include "hal_uart.h"
#include <stdio.h>
#include <string.h>

#define SIM_FIFO_SIZE 512U

typedef struct {
    uint8_t buffer[SIM_FIFO_SIZE];
    size_t head;
    size_t tail;
    size_t count;
    hal_uart_rx_callback_t rx_cb;
} sim_uart_port_t;

static sim_uart_port_t s_ports[HAL_UART_PORT_COUNT];

void hal_uart_init(hal_uart_port_t port, uint32_t baud_rate, hal_uart_rx_callback_t rx_cb)
{
    (void)baud_rate;
    if (port < HAL_UART_PORT_COUNT) {
        memset(&s_ports[port], 0, sizeof(sim_uart_port_t));
        s_ports[port].rx_cb = rx_cb;
    }
}

size_t hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t length)
{
    if (port >= HAL_UART_PORT_COUNT || data == NULL || length == 0) {
        return 0;
    }

    if (port == HAL_UART_PORT_DEBUG) {
        for (size_t i = 0; i < length; ++i) {
            putchar(data[i]);
        }
        fflush(stdout);
        return length;
    }

    return length;
}

bool hal_uart_putc(hal_uart_port_t port, uint8_t byte)
{
    return (hal_uart_write(port, &byte, 1) == 1);
}

bool hal_uart_getc(hal_uart_port_t port, uint8_t *byte)
{
    if (port >= HAL_UART_PORT_COUNT || byte == NULL) {
        return false;
    }

    sim_uart_port_t *p = &s_ports[port];
    if (p->count == 0) {
        return false;
    }

    *byte = p->buffer[p->tail];
    p->tail = (p->tail + 1) % SIM_FIFO_SIZE;
    p->count--;
    return true;
}

bool hal_uart_tx_complete(hal_uart_port_t port)
{
    (void)port;
    return true;
}

void hal_uart_sim_inject_rx(hal_uart_port_t port, const uint8_t *data, size_t length)
{
    if (port >= HAL_UART_PORT_COUNT || data == NULL) {
        return;
    }

    sim_uart_port_t *p = &s_ports[port];
    for (size_t i = 0; i < length; ++i) {
        uint8_t b = data[i];
        if (p->rx_cb != NULL) {
            p->rx_cb(port, b);
        } else if (p->count < SIM_FIFO_SIZE) {
            p->buffer[p->head] = b;
            p->head = (p->head + 1) % SIM_FIFO_SIZE;
            p->count++;
        }
    }
}
