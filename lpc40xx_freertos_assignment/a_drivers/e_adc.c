#include "e_adc.h"

void adc_init(int adc_channel) {
  // Enable power for adc
  LPC_SC->PCONP |= (1 << PCADC);
  LPC_ADC->CR |= (1 << PDN);
  // Enable ADC 0 or 1
}