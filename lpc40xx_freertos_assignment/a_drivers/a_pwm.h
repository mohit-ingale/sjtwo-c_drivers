#ifndef a_pwm_h
#define a_pwm_h
#include <stdint.h>

#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

void a_pwm_init();

void a_pwm_set_duty_cycle(uint8_t duty_cycle,uint8_t pwm1_channel);
#endif