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