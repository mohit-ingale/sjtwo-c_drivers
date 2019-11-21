#ifndef A_MEMORY_H
#define A_MEMORY_H
#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "gpio.h"
#include "semphr.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum which_control {
  a_slave_gpio_control = 1,
  a_slave_memory_control = 2,
  a_slave_acceleration_control = 4
} which_control;

typedef enum which_status {
  a_slave_gpio_status = 1,
  a_slave_memory_valid_status = 2,
  a_slave_acclerometer_valid_status = 4
} which_status;

uint8_t a_memory_write(uint8_t memory_address, uint8_t data);
uint8_t a_memory_read(uint8_t memory_address);
uint8_t a_memory_address_validate(uint8_t memory_address);

xSemaphoreHandle a_acclerometer_signal;
#define MEMORY_SIZE_MAX 100
extern uint8_t my_memory[MEMORY_SIZE_MAX];

#endif