#include "a_i2c.h"
#include "lpc_peripherals.h"

uint8_t previous_state = 0x00;
uint8_t register_addre = 0x00;

uint8_t value_at_memory(uint8_t memory_address) { return a_memory_read(memory_address); }

uint8_t value_to_memory(uint8_t memory_address, uint8_t value) { return a_memory_write(memory_address, value); }

void a_set_error_in_status_reg(uint8_t status_value) { a_write_status_register(status_value); }

void a_i2c_0_isr() { a_i2c_state(LPC_I2C0_BASE); }
void a_i2c_1_isr() { a_i2c_state(LPC_I2C1_BASE); }
void a_i2c_2_isr() { a_i2c_state(LPC_I2C2_BASE); }

void a_i2c_state(LPC_I2C_TypeDef *a_i2c_device_run) {
  uint8_t state = a_i2c_device_run->STAT;
  fprintf(stderr, "Status  = %x\n", state);

  switch (state) {
  case 0x60: // Slave Address Found
    previous_state = state;
    a_i2c_send_ack_for_next_data(a_i2c_device_run);
    break;
  case 0x80: // Data Received
    if (previous_state == 0x60) {
      register_addre = a_i2c_device_run->DAT;
      if (a_memory_address_validate(register_addre)) {
        a_i2c_send_ack_for_next_data(a_i2c_device_run);
      } else {
        a_i2c_send_no_ack_for_next_data(a_i2c_device_run);
      }
    } else {
      if (value_to_memory(register_addre, a_i2c_device_run->DAT)) {
        if ((register_addre == 0x80) || (register_addre == 0x81) || (register_addre == 0x82)) {

        } else {
          register_addre = register_addre + 1;
        }
        a_i2c_send_ack_for_next_data(a_i2c_device_run);
      } else {
        a_i2c_send_no_ack_for_next_data(a_i2c_device_run);
      }
    }
    previous_state = 0x80;

    break;
  case 0x88:
    previous_state = 0x88;
    a_i2c_send_ack_for_next_data(a_i2c_device_run);
    break;
  case 0xA0:
    previous_state = 0xA0;
    a_i2c_send_ack_for_next_data(a_i2c_device_run);
    break;
  case 0xA8: {
    uint8_t data;
    if (previous_state == 0xA0) {
      if (a_memory_address_validate(register_addre)) {
        data = value_at_memory(register_addre);
        if ((register_addre == 0x80) || (register_addre == 0x81) || (register_addre == 0x82)) {

        } else {
          register_addre = register_addre + 1;
        }
        a_i2c_device_run->DAT = data;
      } else {
        data = -1;
      }
      // fprintf(stderr, "Data Sent\n");
    } else {
    }
    previous_state = 0XA8;
    if (data != -1) {
      a_i2c_send_ack_for_next_data(a_i2c_device_run);
    } else {
      a_i2c_send_no_ack_for_next_data(a_i2c_device_run);
    }
  } break;
  case 0xB8: {
    int data;
    if ((previous_state == 0xA8) || (previous_state == 0xB8)) {
      if (a_memory_address_validate(register_addre)) {
        data = value_at_memory(register_addre++);
        a_i2c_device_run->DAT = data;
      } else {
        data = -1;
      }
      // fprintf(stderr, "Incremented Data Sent\n");
    }
    previous_state = 0xB8;
    if (data != -1) {
      a_i2c_send_ack_for_next_data(a_i2c_device_run);
    } else {
      a_i2c_send_no_ack_for_next_data(a_i2c_device_run);
    }
  } break;
  case 0xC0:
    // fprintf(stderr, "Data Recevied by master no ACK STOP\n");
    previous_state = 0xC0;
    a_i2c_send_ack_for_next_data(a_i2c_device_run);
    break;
  case 0xC8:
    // fprintf(stderr, "Final Data Recevied by master ACK STOP\n");
    previous_state = 0XC8;
    a_i2c_send_ack_for_next_data(a_i2c_device_run);
    break;
  default:
    break;
  }
  a_i2c_clear_si_flag(a_i2c_device_run);
}

void a_i2c_clear_si_flag(LPC_I2C_TypeDef *a_i2c_device_run) { a_i2c_device_run->CONCLR = (1 << 3); }

void a_i2c_send_ack_for_next_data(LPC_I2C_TypeDef *a_i2c_device_run) { a_i2c_device_run->CONSET = 0x44; }
void a_i2c_send_no_ack_for_next_data(LPC_I2C_TypeDef *a_i2c_device_run) { a_i2c_device_run->CONCLR = 0x04; }

void temp() { LPC_I2C2->CONSET |= 0x44; }

void a_i2c_slave_init(A_I2C_BUS a_which_i2c_bus) {
  const function__void_f isrs[] = {a_i2c_0_isr, a_i2c_1_isr, a_i2c_2_isr};
  A_I2C_DEVICE a_i2c_device_run;
  __a_i2c__init_handle(a_which_i2c_bus, 0, &a_i2c_device_run);
  __a_i2c_enable(&a_i2c_device_run);
  a_i2c_device_run.lpc_i2c->CONCLR = 0x6C;
  __a_i2c_set_slave_address(&a_i2c_device_run, 0x22);
  lpc_peripheral__enable_interrupt(a_i2c_device_run.i2c_peripheral, isrs[a_i2c_device_run.which_i2c_bus]);
}

void __a_i2c_set_slave_address(A_I2C_DEVICE *a_i2c_device_run, uint8_t slave_address) {
  a_i2c_device_run->lpc_i2c->ADR0 = slave_address;
  a_i2c_device_run->lpc_i2c->ADR1 = 0x00;
  a_i2c_device_run->lpc_i2c->ADR2 = 0x00;
  a_i2c_device_run->lpc_i2c->ADR3 = 0x00;
  a_i2c_device_run->lpc_i2c->MASK0 = 0x00;
  a_i2c_device_run->lpc_i2c->MASK1 = 0x00;
  a_i2c_device_run->lpc_i2c->MASK2 = 0x00;
  a_i2c_device_run->lpc_i2c->MASK3 = 0x00;
  a_i2c_device_run->lpc_i2c->CONSET = 0x44;
}

void __a_i2c__init_handle(A_I2C_BUS a_which_i2c_bus, int is_master, A_I2C_DEVICE *a_i2c_device_run) {
  a_i2c_device_run->which_i2c_bus = a_which_i2c_bus;
  a_i2c_device_run->i2c_x_handle.current_state = 0;
  a_i2c_device_run->i2c_x_handle.is_master = is_master;
  a_i2c_device_run->i2c_x_handle.previous_state = 0;
  a_i2c_device_run->i2c_x_handle.register_address = 0;
}

void __a_i2c_enable(A_I2C_DEVICE *a_i2c_device_run) {
  switch (a_i2c_device_run->which_i2c_bus) {
  case 0:
    // Power
    LPC_SC->PCONP |= (1 << 7);
    // PIN
    LPC_IOCON->P1_30 |= (0b100 << 0);
    LPC_IOCON->P1_31 |= (0b100 << 0);
    // Set Structure
    a_i2c_device_run->lpc_i2c = LPC_I2C0_BASE;
    a_i2c_device_run->i2c_peripheral = LPC_PERIPHERAL__I2C0;
    break;
  case 1:
    // Power
    LPC_SC->PCONP |= (1 << 19);
    // PIN
    LPC_IOCON->P0_0 = (0b011 << 0); // SDA
    LPC_IOCON->P0_1 = (0b011 << 0); // SCL
    // LPC_IOCON->P0_19 |= (1<<0b011);      // SDA
    // LPC_IOCON->P0_20 |= (1<<0b011);         // SCL
    // Set Structure
    a_i2c_device_run->lpc_i2c = LPC_I2C1_BASE;
    a_i2c_device_run->i2c_peripheral = LPC_PERIPHERAL__I2C1;
    break;
  case 2:
    // POWER
    LPC_SC->PCONP |= (1 << 26);
    // PIN
    LPC_IOCON->P0_10 |= (0b010 << 0); // SDA
    LPC_IOCON->P0_11 |= (0b010 << 0); // SCL
    // Set Structure
    a_i2c_device_run->lpc_i2c = LPC_I2C2_BASE;
    a_i2c_device_run->i2c_peripheral = LPC_PERIPHERAL__I2C2;
    break;
  default:
    break;
  }
}
