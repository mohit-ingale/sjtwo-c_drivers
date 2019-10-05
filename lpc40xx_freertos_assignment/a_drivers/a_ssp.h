#ifndef a_ssp_h
#define a_ssp_h

#include "stdio.h"
#include "lpc40xx.h"
#include "my_gpio.h"


typedef enum A_SSP_CHIP_SELECT{INIT_FLASH = 0, ASSERT,DEASSERT}A_SSP_CHIP_SELECT;

typedef struct external_flash_signature{
    uint8_t manufacturer_id;
    uint8_t device_id_1;
    uint8_t device_id_2;
    uint8_t extended_device_information;
}EXTERNAL_FLASH_SIGNATURE;

void a_ssp_init(uint32_t ssp_interface);
uint8_t a_ssp_send_receive_1byte(uint8_t data);
EXTERNAL_FLASH_SIGNATURE a_ssp_read_device_signature();
#endif