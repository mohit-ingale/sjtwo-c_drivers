#ifndef a_ssp_h
#define a_ssp_h

#include "lpc40xx.h"
#include "my_gpio.h"
#include "stdio.h"
#define A_CPU_CLOCK_FREQUENCY 96 * 1000 * 1000

typedef enum A_SSP_CHIP_SELECT { INIT_FLASH = 0, ASSERT, DEASSERT } A_SSP_CHIP_SELECT;

typedef struct adesco_status_register_1 {
  uint8_t bpl : 1;
  uint8_t : 1; // Reserved
  uint8_t epe : 1;
  uint8_t wpp : 1;
  uint8_t activity : 1;
  uint8_t bp0 : 1;
  uint8_t wel : 1;
  uint8_t ready : 1;
} ADESCO_STATUS_REGISTER_1;

typedef struct adesco_status_register_2 {
  uint8_t : 1;
  uint8_t : 1; // Reserved
  uint8_t : 1;
  uint8_t RSTE : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t : 1;
  uint8_t ready : 1;
} ADESCO_STATUS_REGISTER_2;

typedef struct external_flash_signature {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_information;
} EXTERNAL_FLASH_SIGNATURE;

typedef struct status_register_adesto {
  ADESCO_STATUS_REGISTER_1 status_register_byte1;
  ADESCO_STATUS_REGISTER_2 status_register_byte2;
} STATUS_REGISTER_ADESTO;

void a_ssp_init(uint32_t ssp_interface, uint32_t clock_frequency_in_mhz);
uint8_t a_ssp_send_receive_1byte(uint8_t data);
EXTERNAL_FLASH_SIGNATURE a_ssp_read_device_signature();
STATUS_REGISTER_ADESTO a_ssp_read_device_status();
void print_status(const STATUS_REGISTER_ADESTO *a_external_flash_status);
#endif