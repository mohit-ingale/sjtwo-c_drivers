#include "a_adc.h"
#include "lpc40xx.h"


void a_adc_init(int burst_mode){
    LPC_IOCON->P0_25 &= ~(1<<7);
    LPC_IOCON->P1_30 &= ~(1<<7);
    LPC_IOCON->P1_31 &= ~(1<<7);
    LPC_IOCON->P2_0 &= ~(7 << 0);
    LPC_IOCON->P2_1 &= ~(7 << 0);
    LPC_IOCON->P2_2 &= ~(7 << 0);

    LPC_IOCON->P2_0 = 1;
    LPC_IOCON->P2_1 = 1;
    LPC_IOCON->P2_2 = 1 ;
    // LPC_IOCON->P0_25 |= (1<<7);
    // LPC_IOCON->P1_30 |= (1<<7);
    // LPC_IOCON->P1_31 |= (1<<7);
    LPC_SC->PCONP |= (1<<12);
    LPC_ADC->CR = 0;
    LPC_ADC->CR = (1<<21) | (1<<9);// | (1<<16);
}

void a_adc_start(int adc_number){
    uint8_t adc_number_mask = 0x00;
    adc_number_mask |= (1<<adc_number);
    LPC_ADC->CR |= adc_number_mask;
    LPC_ADC->CR |= (1<<24);
}

uint16_t a_get_adc_data(int adc_number){
    uint16_t l_adc_data_return = 0;
    uint8_t in_if = 0;
    do{
        if((LPC_ADC->DR[adc_number]) & (1<<31)){
            l_adc_data_return = (LPC_ADC->DR[adc_number] & (0xffff));
            l_adc_data_return = l_adc_data_return >> 4;
            LPC_ADC->CR &= ~(1<<adc_number);
            in_if = 1;
        }
    }
    while(in_if == 0);
    return l_adc_data_return;
}