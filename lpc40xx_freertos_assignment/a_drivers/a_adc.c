#include "adc.h"
#include "lpc40xx.h"


void a_adc_init(int channel_number, int busrt_mode){
    LPC_IOCON->P0_25 &= ~(1<<7);
    LPC_SC->PCONP |= (1<<12);
    LPC_ADC->CR = (1<<2) | (1<<21) | (1<<9);
}

void a_adc_start(){
    LPC_ADC->CR |= (1<<24);
}

uint16_t a_get_adc_data(){
    uint16_t l_adc_data_return;
    if((LPC_ADC->DR[2]) & (1<<31)){
        l_adc_data_return = (LPC_ADC->DR[2] & (0xffff));
        l_adc_data_return = l_adc_data_return >> 4;
    }
    return l_adc_data_return;
}