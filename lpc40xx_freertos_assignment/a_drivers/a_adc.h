#ifndef a_adc.h
#define a_adc.h
#include "lpc40xx.h"

void a_adc_init(int channel_number, int busrt_mode);
void a_adc_start();
uint16_t a_get_adc_data();

#endif