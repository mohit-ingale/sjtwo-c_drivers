#include "a_uart.h"

const int __a_uart_power[5] = {[0] = 3, [1] = 4, [2] = 24, [3] = 25, [4] = 8};

#define DLAB 7
LPC_UART_TypeDef *__a_lpc_uart[5] = {
    [0] = LPC_UART0, [1] = LPC_UART1, [2] = LPC_UART2, [3] = LPC_UART3, [4] = LPC_UART4};

static void __a_uart_pin_init(UART_DEVICE uart_device) {
  switch (uart_device) {
  case UART0:
    LPC_IOCON->P0_2 &= ~(0b001); // tx
    LPC_IOCON->P0_3 &= ~(0b001); // rx

    LPC_IOCON->P0_2 |= 0b001; // tx
    LPC_IOCON->P0_3 |= 0b001; // rx
    break;
  case UART1:
    LPC_IOCON->P0_15 &= ~(0b111); // tx
    LPC_IOCON->P0_16 &= ~(0b111); // rx

    LPC_IOCON->P0_15 |= 0b001; // tx
    LPC_IOCON->P0_16 |= 0b001; // rx
    break;
  case UART2:
    LPC_IOCON->P0_10 &= ~(0b111); // tx
    LPC_IOCON->P0_11 &= ~(0b111); // rx

    LPC_IOCON->P0_10 |= 0b001; // tx
    LPC_IOCON->P0_11 |= 0b001; // rx
    break;
  case UART3:
    LPC_IOCON->P4_28 &= ~(0b111); // tx
    // LPC_IOCON->P4_29 &= ~(0b111); // rx

    LPC_IOCON->P4_28 |= 0b010; // tx
    // LPC_IOCON->P4_29 |= 0b010; // rx

    // LPC_IOCON->P0_0 &= ~(0b111); // tx
    LPC_IOCON->P0_1 &= ~(0b111); // rx

    // LPC_IOCON->P0_0 |= 0b010; // tx
    LPC_IOCON->P0_1 |= 0b010; // rx
    break;
  case UART4:
    LPC_IOCON->P5_4 &= ~(0b111); // tx
    LPC_IOCON->P2_9 &= ~(0b111); // rx

    LPC_IOCON->P5_4 |= 0b100; // tx
    LPC_IOCON->P2_9 |= 0b011; // rx
    break;
  default:
    LPC_IOCON->P0_2 &= ~(0b111); // tx
    LPC_IOCON->P0_3 &= ~(0b111); // rx

    LPC_IOCON->P0_2 |= 0b001; // tx
    LPC_IOCON->P0_3 |= 0b001; // rx
    break;
  }
  return;
}

static void __a_uart_baudrate_set(UART_DEVICE uart_device, int baudrate) {
  {
    const uint16_t baud_rate_setter =
        (uint16_t)(peripheral_clock / (16 * baudrate)); // Will implement the logic to calculate the offset later
    // Step 2 - Set Peripheral clock (we will not change default clock rate)
    // Step 3 - Set DLAB bit for Setting the baud rate
    __a_lpc_uart[uart_device]->LCR |= (1 << DLAB);
    // Step 3 - Set Baud Rate
    __a_lpc_uart[uart_device]->DLM = (uint8_t)(baud_rate_setter >> 8);
    __a_lpc_uart[uart_device]->DLL = (uint8_t)baud_rate_setter;
    // Clear DLAB Bit
    __a_lpc_uart[uart_device]->LCR = BIT_8;
  }
}

static void __a_uart_init(UART_DEVICE uart_device, int baudrate) {
  // Step 1 - Enable power for respective uart device
  LPC_SC->PCONP |= (1 << __a_uart_power[uart_device]);
  // Step 2 - Set Peripheral clock (we will not change default clock rate)
  // Step 3 - Set the baud rate
  __a_uart_baudrate_set(uart_device, baudrate);
  // step 5 - Intialize pin;
  __a_uart_pin_init(uart_device);
}

static void __a_uart_tx(UART_DEVICE uart_device, uint8_t data) {

  while (!(__a_lpc_uart[uart_device]->LSR & (1 << 5)))
    ;
  __a_lpc_uart[uart_device]->THR = data;

  while (!(__a_lpc_uart[uart_device]->LSR & (1 << 5)))
    ;
  return;
}

static void __a_uart_rx(UART_DEVICE uart_device, uint8_t *data) {
  while (!(__a_lpc_uart[uart_device]->LSR & (1 << 0)))
    ;
  *data = __a_lpc_uart[uart_device]->RBR;
  return;
}

static void __a_uart_rx_enable(UART_DEVICE uart_device, function__void_f uart_rx_in_interrupt) {
  A_PERIPHERAL_INTERRUPT a_interrupt_rx = {
      uart_device + 5,
      uart_rx_in_interrupt,
      0,
      0,
  };
  __a_lpc_uart[uart_device]->IER |= (1 << 0);
  a_enable_interrupt(&a_interrupt_rx);
}

uint8_t a_uart_interrupt_which(UART_DEVICE uart_device) { return ((__a_lpc_uart[uart_device]->IIR & 0xff)); }

void a_uart_rx_enable(UART_DEVICE uart_device, function__void_f uart_rx_in_interrupt) {
  __a_uart_rx_enable(uart_device, uart_rx_in_interrupt);
}
void a_uart_init(UART_DEVICE uart_device, int baudrate) { __a_uart_init(uart_device, baudrate); }

void a_uart_tx(UART_DEVICE uart_device, uint8_t data) { __a_uart_tx(uart_device, data); }
void a_uart_rx(UART_DEVICE uart_device, uint8_t *data) { __a_uart_rx(uart_device, data); }