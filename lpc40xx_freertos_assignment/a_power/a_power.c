#include "a_power.h"
#include "a_adc.h"

void get_current_board_init() {
  a_adc_init(1);
  a_adc_start(2);
}
uint16_t get_current_board_raw() { return a_get_adc_data(2); }
