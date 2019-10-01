#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"

#include "board_io.h"
#include "delay.h"

#include "uart.h"
// #include "gpio.h"
// #include "uart.h"
// #include "uart_printf.h"

#include "my_gpio.h"
#include "a_interrupt.h"
#include "semphr.h"
#include "a_adc.h"
#include "pwm1.h"
#include "queue.h"


// static void a_blink_task(void *params);
// static void a_read_switch(void *params);
/*
static void uart_task(void *params);
*/
static void uart0_init(void);

void a_gpio_producer_isr(void);
static void a_task_gpio_consumer(void *params);
extern void a_gpio_isr(void);

static void a_task_adc_run(void *params);
static void a_task_pwm_run(void *params);

static struct IO_PORT_PIN outpin1, outpin2;
static struct IO_PORT_PIN inpin1, inpin2;

static A_PERIPHERAL_INTERRUPT interrupt_type_gpio_1,interrupt_type_gpio_2;
static SemaphoreHandle_t switch_signal=NULL;
QueueHandle_t r_pwm_duty_cycle_queue,g_pwm_duty_cycle_queue,b_pwm_duty_cycle_queue;

int main(void) {
  switch_signal = xSemaphoreCreateBinary();
  uart0_init();
  my_gpio_init(1,26,OUT,&outpin1);   //Initialize Port 1 Pin 26 as output
  my_gpio_init(2,3,OUT,&outpin2);   //Initialize Port 2 Pin 7 as output
  my_gpio_init(0,29,IN,&inpin1);
  my_gpio_init(0,30,IN,&inpin2);
  r_pwm_duty_cycle_queue = xQueueCreate(1,sizeof(uint32_t));
  g_pwm_duty_cycle_queue = xQueueCreate(1,sizeof(uint32_t));
  b_pwm_duty_cycle_queue = xQueueCreate(1,sizeof(uint32_t));
  pwm1__init_single_edge(1000);
  // LPC_IOCON->P2_0 = (1<<1);
  // LPC_IOCON->P2_1 = (1<<1);
  // LPC_IOCON->P2_2 = (1<<1);
  gpio__construct_with_function(gpio__port_2,0,gpio__function_1);
  gpio__construct_with_function(gpio__port_2,1,gpio__function_1);
  gpio__construct_with_function(gpio__port_2,2,gpio__function_1);
  
  pwm1__set_duty_cycle(0,75);
  pwm1__set_duty_cycle(1,85);
  pwm1__set_duty_cycle(2,50);
  // uart0_init();
  // a_interrupt_create(&interrupt_type_gpio_1,LPC_PERIPHERAL__GPIO,a_gpio_producer_isr,&inpin1,RISING_EDGE);
  // a_interrupt_create(&interrupt_type_gpio_2,LPC_PERIPHERAL__GPIO,a_gpio_isr,&inpin2,FALLING_EDGE);
  
  // xTaskCreate(a_task_gpio_consumer, "led_toogle_isr", (2048U / sizeof(void *)), (void *)&outpin1, PRIORITY_LOW, NULL);
  xTaskCreate(a_task_adc_run, "adc_task", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(a_task_pwm_run, "pwm_task", (2048U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  vTaskStartScheduler();

  return 0;
}

static void a_task_pwm_run(void *params){
  uint16_t dutycycle = 0;
  while(1){
    if(xQueueReceive(r_pwm_duty_cycle_queue,&dutycycle,100)){
      pwm1__set_duty_cycle(0,dutycycle);
    }
    if(xQueueReceive(g_pwm_duty_cycle_queue,&dutycycle,100)){
      pwm1__set_duty_cycle(1,dutycycle);
    }
    if(xQueueReceive(b_pwm_duty_cycle_queue,&dutycycle,100)){
      pwm1__set_duty_cycle(2,dutycycle);
    }
  }
}

static void a_task_adc_run(void *params){
  uint16_t l_a_adc_data;
  uint16_t dutycycle = 50;
  a_adc_init(1);
  while(1){
    a_adc_start(2);
    l_a_adc_data = a_get_adc_data(2);
    dutycycle = (l_a_adc_data * 100)/4095;
    // uart_printf(UART__0,"ADC_DATA = %d\n",dutycycle);
    xQueueSend(r_pwm_duty_cycle_queue,(void *)&dutycycle,100);
     a_adc_start(4);
    l_a_adc_data = a_get_adc_data(4);
    dutycycle = (l_a_adc_data * 100)/4095;
    // uart_printf(UART__0,"ADC_DATA = %d\n",dutycycle);
    xQueueSend(g_pwm_duty_cycle_queue,(void *)&dutycycle,100);
     a_adc_start(5);
    l_a_adc_data = a_get_adc_data(5);
    dutycycle = (l_a_adc_data * 100)/4095;
    // uart_printf(UART__0,"ADC_DATA = %d\n",dutycycle);
    xQueueSend(b_pwm_duty_cycle_queue,(void *)&dutycycle,100);
    // vTaskDelay(100);
  }
}

extern void a_gpio_isr(void){
  my_gpio_toggle(&outpin2);
  //a_interrupt_clear(&inpin1);
}

void a_gpio_producer_isr(void){
   long semaphore_yld = 0;
  //uart_printf(UART__0, "a_gpio_producer_isr : Interrupt Occured");  /* will not work cannot write uart_prinf in isr"
  xSemaphoreGiveFromISR(switch_signal,&semaphore_yld);
  // LPC_GPIOINT->IO0IntClr |= (1<<29);
  portYIELD_FROM_ISR(semaphore_yld);
}

static void a_task_gpio_consumer(void *params){
  IO_PORT_PIN *op_pin = (IO_PORT_PIN *) params;
  while(1){
    if(xSemaphoreTake(switch_signal,(TickType_t) 10) == pdTRUE){
      my_gpio_toggle(op_pin);
    }
    vTaskDelay(100);
  }
}



/*
------------------NOT NEEDED FOR THIS ASSIGNMENT-----------------------
*/
/*

static void a_read_switch(void *params){
  struct IO_PORT_PIN *in_pin = (struct IO_PORT_PIN *)params;
  while(1){
    if(my_gpio_get(in_pin)){
      while(my_gpio_get(in_pin));
      switch_status = 1;
    }
    vTaskDelay(50);
  }
}

static void a_blink_task(void *params) {
  struct IO_PORT_PIN *led_pin = (struct IO_PORT_PIN *)params;
      while(1){
        if(switch_status){
        my_gpio_toggle(led_pin);
        switch_status = 0;
        }
        vTaskDelay(50);
      }
}


static void uart_task(void *params) {
  TickType_t previous_tick = 0;

  while (true) {
    vTaskDelayUntil(&previous_tick, 500);

    // Wait until the data is fully printed before moving on
    const unsigned ticks = xTaskGetTickCount();
    uart_printf__polled(UART__0, "%6u: Hello world\n", ticks);

    // This deposits data to an outgoing queue and doesn't block the CPU
    uart_printf(UART__0, " ... and a more efficient printf...\n");
  }
}
*/
static void uart0_init(void) {
  // Note: PIN functions are initialized by board_io__initialize() for P0.2(Tx) and P0.3(Rx)
  uart__init(UART__0, clock__get_peripheral_clock_hz(), 115200);

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  QueueHandle_t tx_queue = xQueueCreate(128, sizeof(char));
  QueueHandle_t rx_queue = xQueueCreate(32, sizeof(char));
  uart__enable_queues(UART__0, tx_queue, rx_queue);
}

