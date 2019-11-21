#include "a_memory.h"
#include <stdio.h>

// Acceleration 0x83
// GPIO is at 0x82
// Control 0x81
// Status 0x80

/**
 *
 *
 **/

typedef struct control_memory_reg {
  uint8_t gpio_control_board : 1;
  uint8_t memory_control : 1;
  uint8_t acceleration_control : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;

} control_memory_reg;

typedef struct status_memory_reg {
  uint8_t gpio_status_board : 1;
  uint8_t is_memory_address_invalid : 1;
  uint8_t acceleration_status : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;

} status_memory_reg;

static status_memory_reg status_reg = {0};
static control_memory_reg control_reg = {0};
static gpio_s led0;

// #define MEMORY_SIZE_MAX 100
uint8_t my_memory[MEMORY_SIZE_MAX] = {0};

void a_control_register_changed_take_action(uint8_t data);
uint8_t a_slave_gpio_action(uint8_t data, int is_write);
uint8_t a_slave_memory_action(uint8_t memory_address, uint8_t data, uint8_t is_write);
uint8_t a_data_received_take_action(uint8_t memory_address, uint8_t data, uint8_t is_write);
uint8_t a_read_status_register();
uint8_t a_read_control_register();
void a_write_status_register(uint8_t status_value);
void a_write_control_register(uint8_t control_value);
uint8_t a_memory_address_validate(uint8_t memory_address);
uint8_t a_memory_write(uint8_t memory_address, uint8_t data);
uint8_t a_memory_read(uint8_t memory_address);

void a_control_register_changed_take_action(uint8_t data) {
  if (a_read_control_register() & a_slave_gpio_control) {
    fprintf(stderr, "Led initilialized\n");
    led0 = gpio__construct_with_function(1, 18, GPIO__FUNCITON_0_IO_PIN);
  }
  if (a_read_control_register() & a_slave_memory_control) {
  }
  if (a_read_control_register() & a_slave_acceleration_control) {
    fprintf(stderr, "Acceleration_sensor initilialized\n");
    xSemaphoreGiveFromISR(a_acclerometer_signal, NULL);
  }
}

uint8_t a_slave_gpio_action(uint8_t data, int is_write) {
  if (is_write) {
    if (data == 1) {
      fprintf(stderr, "gpio set\n");
      gpio__reset(led0);
    } else {
      fprintf(stderr, "gpio reset\n");
      gpio__set(led0);
    }
    return 1;
  } else {
    fprintf(stderr, "gpio get\n");
    return gpio__get(led0);
  }
}

uint8_t a_acclerometer_action(uint8_t which_register) {
  return my_memory[which_register];
}

uint8_t a_slave_memory_action(uint8_t memory_address, uint8_t data, uint8_t is_write) {
  if (a_memory_address_validate(memory_address)) {
    if (is_write) {
      my_memory[memory_address - 1] = data;
      return 1;
    } else {
      if (memory_address >= 0 && memory_address <= MEMORY_SIZE_MAX) {
        return my_memory[memory_address - 1];
      } else {
        fprintf(stderr, "Error reading\n");
        return -1;
      }
    }
  } else
    return 0;
}

uint8_t a_data_received_take_action(uint8_t memory_address, uint8_t data, uint8_t is_write) {
  uint8_t ret_val = 0;
  if (memory_address == 0x80) {
    return a_read_status_register();
  } else if (memory_address == 0x81) {
    if (is_write) {
      a_write_control_register(data);
      return 1;
    } else {
      return a_read_control_register();
    }
  } else if (memory_address == 0x82 && (a_read_control_register() & a_slave_gpio_control)) {
    return a_slave_gpio_action(data, is_write);
  } else if ((memory_address >= MEMORY_SIZE_MAX - 6) && (a_read_control_register() & a_slave_acceleration_control)) {
    return a_acclerometer_action(memory_address);
  } else if (a_read_control_register() & a_slave_memory_control) {
    return a_slave_memory_action(memory_address, data, is_write);
  } else {
    return 0;
  }
}

uint8_t a_read_status_register() {
  uint8_t local_status = 0;
  local_status |= (status_reg.gpio_status_board << 0);
  local_status |= (status_reg.is_memory_address_invalid << 1);
  local_status |= (status_reg.acceleration_status << 2);
  return local_status;
}

uint8_t a_read_control_register() {
  uint8_t local_control_reg = 0;
  local_control_reg |= (control_reg.gpio_control_board << 0);
  local_control_reg |= (control_reg.memory_control << 1);
  local_control_reg |= (control_reg.acceleration_control << 2);
  return local_control_reg;
}

void a_write_status_register(uint8_t status_value) {
  (status_value & a_slave_gpio_status) ? (status_reg.gpio_status_board = 1) : (status_reg.gpio_status_board = 0);
  (status_value & a_slave_memory_valid_status) ? (status_reg.is_memory_address_invalid = 1)
                                               : (status_reg.is_memory_address_invalid = 0);
  (status_value & a_slave_acclerometer_valid_status) ? (status_reg.acceleration_status = 1)
                                                     : (status_reg.acceleration_status = 0);
}

void a_write_control_register(uint8_t control_value) {
  fprintf(stderr, "writing control reg = %d\n", control_value);
  (control_value & a_slave_gpio_control) ? (control_reg.gpio_control_board = 1) : (control_reg.gpio_control_board = 0);
  (control_value & a_slave_memory_control) ? (control_reg.memory_control = 1) : (control_reg.memory_control = 0);
  (control_value & a_slave_acceleration_control) ? (control_reg.acceleration_control = 1)
                                                 : (control_reg.acceleration_control = 0);
  a_control_register_changed_take_action(control_value);
}

uint8_t a_memory_address_validate(uint8_t memory_address) {
  if (((memory_address >= 0) && (memory_address < MEMORY_SIZE_MAX)) || (memory_address == 0x82)) {
    a_write_status_register(0x00);
    return 1;
  } else if (memory_address == 0x80 || memory_address == 0x81) {
    return 1;
  } else {
    a_write_status_register(0x02);
    return 0;
  }
}

uint8_t a_memory_write(uint8_t memory_address, uint8_t data) {
  fprintf(stderr, "Writing to memory = %d\n", memory_address);
  return a_data_received_take_action(memory_address, data, 1);
}

uint8_t a_memory_read(uint8_t memory_address) {
  return a_data_received_take_action(memory_address, 0, 0);
  // if (memory_address == 0x80) {
  //   return a_read_status_register();
  // }
  // else if (memory_address == 0x81) {
  //   return a_read_control_register();
  // }
  // else if (memory_address <= MEMORY_SIZE_MAX) {
  //   return my_memory[memory_address - 1];
  // } else {
  //   fprintf(stderr, "Error reading\n");
  //   return -1;
  // }
}