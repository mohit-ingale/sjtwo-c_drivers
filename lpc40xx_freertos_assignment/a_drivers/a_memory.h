#ifndef A_MEMORY_H
#define A_MEMORY_H
#include "FreeRTOS.h"
#include "board_io.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum which_control { a_slave_gpio_control = 1, a_slave_memory_control = 2 } which_control;

typedef enum which_status { a_slave_gpio_status = 1, a_slave_memory_valid_status = 2 } which_status;

uint8_t a_memory_write(uint8_t memory_address, uint8_t data);
uint8_t a_memory_read(uint8_t memory_address);
uint8_t a_memory_address_validate(uint8_t memory_address);

#endif