#include "a_ssp.h"
static IO_PORT_PIN a_spi_cs,a_spi_fake_cs;

void a_ssp_init(uint32_t ssp_interface,uint32_t clock_frequency_in_mhz){
    if(ssp_interface == 2){
        LPC_SC->PCONP |= (1<<20);   //Turn on power for SSP2
        // LPC_SC->PCLKSEL |= (2 << 1);    //Divide clock by two so that spi will divide clock further by 2 and resultant speed will be 24mhz which is standard for sd card and pcb design
        LPC_IOCON->P1_0 = (0b100<<0);
        LPC_IOCON->P1_1 = (0b100<<0);
        LPC_IOCON->P1_4 = (0b100<<0);
        LPC_SSP2->CR0 |= (0b111<<0);
        LPC_SSP2->CPSR = (A_CPU_CLOCK_FREQUENCY/(1000 * 1000))/(clock_frequency_in_mhz);
        a_ssp_chip_select(1,10,INIT_FLASH);
    }
}

uint8_t a_ssp_send_receive_1byte(uint8_t data){
    uint16_t temp;
    while(!(LPC_SSP2->SR & (1<<1)));
    LPC_SSP2->DR = data;
    while(!(LPC_SSP2->SR & (1<<2)));
    temp = (LPC_SSP2->DR & 0xFFFF);
    return temp;
}

void a_ssp_chip_select(int port, int pin, A_SSP_CHIP_SELECT state){
    if(state == INIT_FLASH){
        my_gpio_init(port,pin,OUT,&a_spi_cs);
        my_gpio_init(0,6,OUT,&a_spi_fake_cs);
    }
    else if(state == ASSERT){
        my_gpio_clr(&a_spi_cs);
        my_gpio_clr(&a_spi_fake_cs);
    }
    else if(state == DEASSERT){
        my_gpio_set(&a_spi_cs);
        my_gpio_set(&a_spi_fake_cs);
    }

}

EXTERNAL_FLASH_SIGNATURE a_ssp_read_device_signature(){
    uint8_t data = 0;
    EXTERNAL_FLASH_SIGNATURE a_external_flash_signature;
    STATUS_REGISTER_ADESTO a_external_flash_status;
    a_external_flash_status = a_ssp_read_device_status();
    if(!(a_external_flash_status.status_register_byte2 & 0x01)){
        a_ssp_chip_select(1,10,ASSERT);
        (void) a_ssp_send_receive_1byte(0x9F);
        data = a_ssp_send_receive_1byte(0xff);
        a_external_flash_signature.manufacturer_id = data;
        data = a_ssp_send_receive_1byte(0xff);
        a_external_flash_signature.device_id_1 = data;
        data = a_ssp_send_receive_1byte(0xff);
        a_external_flash_signature.device_id_2 = data;
        data = a_ssp_send_receive_1byte(0xff);
        a_external_flash_signature.extended_device_information = data;
        a_ssp_chip_select(1,10,DEASSERT);
    }
    else{
        fprintf(stderr, "Device not ready for reading\n");
    }
    return a_external_flash_signature;
}

STATUS_REGISTER_ADESTO a_ssp_read_device_status(){
    uint8_t data = 0;
    STATUS_REGISTER_ADESTO a_external_flash_status;
    a_ssp_chip_select(1,10,ASSERT);
    (void) a_ssp_send_receive_1byte(0x05);
    data = a_ssp_send_receive_1byte(0xff);
    a_external_flash_status.status_register_byte1 = data;
    data = a_ssp_send_receive_1byte(0xff);
    a_external_flash_status.status_register_byte2 = data;
    a_ssp_chip_select(1,10,DEASSERT);
    return a_external_flash_status;
}