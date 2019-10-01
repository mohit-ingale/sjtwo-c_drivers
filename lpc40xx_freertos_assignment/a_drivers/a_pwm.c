#include "a_pwm.h"
#include "lpc40xx.h"

void a_pwm_init(uint32_t pwm_frequency){
    // pwm_frequency = 50;
    LPC_SC->PCONP |= (1<<6);
    //LPC_SC->PCLKSEL |= (1<<1);
    LPC_IOCON->P2_0 &= ~(7 << 0);
    LPC_IOCON->P2_1 &= ~(7 << 0);
    LPC_IOCON->P2_2 &= ~(7 << 0);

    LPC_IOCON->P2_0 = 1;
    LPC_IOCON->P2_1 = 1;
    LPC_IOCON->P2_2 = 1 ;

    const uint32_t match_reg_value = ((96*1000*1000) / pwm_frequency);

    if (match_reg_value > 0) {
        LPC_PWM1->MR0 = (match_reg_value - 1);
    }

    LPC_PWM1->MCR |= (1 << 1);                              
    LPC_PWM1->TCR = (1 << 0) | (1 << 3);
    LPC_PWM1->PCR |= (0x3F << 9); 
}


void a_pwm_set_duty_cycle(uint8_t pwm1_channel,uint8_t duty_cycle){
    const uint32_t mr0_reg_val = LPC_PWM1->MR0;
  const uint32_t match_reg_value = (mr0_reg_val * duty_cycle) / 100;

  switch (pwm1_channel) {
  case 0:
    LPC_PWM1->MR1 = match_reg_value;
    break;
  case 1:
    LPC_PWM1->MR2 = match_reg_value;
    break;
  case 2:
    LPC_PWM1->MR3 = match_reg_value;
    break;
  case 4:
    LPC_PWM1->MR5 = match_reg_value;
    break;
  case 5:
    LPC_PWM1->MR6 = match_reg_value;
    break;
  default:
    break;
  }

  LPC_PWM1->LER |= (1 << (pwm1_channel + 1)); 
}