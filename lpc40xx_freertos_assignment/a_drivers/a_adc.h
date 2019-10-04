#ifndef a_adc_h
#define a_adc_h
#include "lpc40xx.h"

void a_adc_init(int busrt_mode);
void a_adc_start(int adc_number);
uint16_t a_get_adc_data(int adc_number);

#endif