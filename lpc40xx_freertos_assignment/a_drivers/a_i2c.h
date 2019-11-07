#ifndef A_I2C_H
#define A_I2C_H

#include "FreeRTOS.h"
#include "a_memory.h"
#include "board_io.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdio.h>

typedef enum { a_i2c_bus_0, a_i2c_bus_1, a_i2c_bus_2 } A_I2C_BUS;
typedef enum { a_i2c_power_0 = 7, a_i2c_power_1 = 19, a_i2c_power_2 = 26 } A_POWER_I2C_DEVICE;

typedef struct {
  uint8_t current_state;
  uint8_t previous_state;
  uint8_t is_master;
  uint32_t register_address;
} A_I2C_HANDLE;

typedef struct {
  A_I2C_HANDLE i2c_x_handle;
  A_I2C_BUS which_i2c_bus;
  LPC_I2C_TypeDef *lpc_i2c;
  lpc_peripheral_e i2c_peripheral;
} A_I2C_DEVICE;

static gpio_s led0;

void a_i2c_clear_si_flag(LPC_I2C_TypeDef *a_i2c_device_run);
void a_i2c_send_ack_for_next_data(LPC_I2C_TypeDef *a_i2c_device_run);
void a_i2c_send_no_ack_for_next_data(LPC_I2C_TypeDef *a_i2c_device_run);
void a_i2c_slave_init(A_I2C_BUS a_which_i2c_bus);
uint8_t value_at_memory(uint8_t memory_address);
uint8_t value_to_memory(uint8_t memory_address, uint8_t value);

#endif