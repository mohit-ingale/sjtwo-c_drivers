#ifndef A_UART_H
#define A_UART_H
#include "a_interrupt.h"
#include "lpc40xx.h"

#define peripheral_clock 96000000

typedef enum A_UART_DEVICE { UART0 = 0, UART1, UART2, UART3, UART4 } UART_DEVICE;

typedef enum A_UART_LENGTH { BIT_5 = 0, BIT_6, BIT_7, BIT_8 } UART_WORD_LENGTH;

void a_uart_init(UART_DEVICE uart_device, int baudrate);

void a_uart_tx(UART_DEVICE uart_device, uint8_t data);
void a_uart_rx(UART_DEVICE uart_device, uint8_t *data);
void a_uart_rx_enable(UART_DEVICE uart_device, function__void_f uart_rx_in_interrupt);
uint8_t a_uart_interrupt_which(UART_DEVICE uart_device);

#endif