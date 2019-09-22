#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "delay.h"
#include "gpio.h"

#include "my_gpio.h"

#include "uart.h"
#include "uart_printf.h"

#include "a_interrupt.h"
#include "semphr.h"

// static void a_blink_task(void *params);
// static void a_read_switch(void *params);

static void uart_task(void *params);
static void uart0_init(void);

void a_gpio_producer_isr(void);
static void a_task_gpio_consumer(void *params);
extern void a_gpio_isr(void);

static struct IO_PORT_PIN outpin1, outpin2;
static struct IO_PORT_PIN inpin1, inpin2;
static uint8_t switch_status = 0;


static A_PERIPHERAL_INTERRUPT interrupt_type_gpio;
static lpc_peripheral_e gpio_interrupt = LPC_PERIPHERAL__GPIO;

static SemaphoreHandle_t switch_signal=NULL;


int main(void) {
   switch_signal = xSemaphoreCreateBinary();
  my_gpio_init(1,26,OUT,&outpin1);   //Initialize Port 1 Pin 26 as output
  //my_gpio_init(2,3,OUT,&outpin2);   //Initialize Port 2 Pin 7 as output
  my_gpio_init(0,29,IN,&inpin1);
  uart0_init();
  //a_interrupt_create(&interrupt_type_gpio,gpio_interrupt,a_gpio_isr,&inpin1,FALLING_EDGE);
  //a_interrupt_init(&interrupt_type_gpio);
   lpc_peripheral__enable_interrupt(gpio_interrupt,a_gpio_producer_isr);
   LPC_GPIOINT->IO0IntEnR |= (1<<29);
   NVIC_EnableIRQ(GPIO_IRQn);
  
  xTaskCreate(a_task_gpio_consumer, "led_toogle_isr", (2048U / sizeof(void *)), (void *)&outpin1, PRIORITY_LOW, NULL);
  //xTaskCreate(a_read_switch, "sw1", (512U / sizeof(void *)), (void *)&inpin1, PRIORITY_LOW, NULL);
//while(1);
  // printf() takes more stack space
  //xTaskCreate(uart_task, "uart", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
//while(1);
  vTaskStartScheduler();

  /**
   * vTaskStartScheduler() should never return.
   *
   * Otherwise, it returning indicates there is not enough free memory or scheduler was explicitly terminated
   * CPU will now halt forever at this point.
   */

  return 0;
}

extern void a_gpio_isr(void){
  uart_printf(UART__0, "a_gpio_producer_isr : Interrupt Occured\n");
  my_gpio_toggle(&outpin1);
  LPC_GPIOINT->IO0IntClr |= (1<<29);
    //a_interrupt_clear(&inpin1);
}

void a_gpio_producer_isr(void){
   long yield = 0;
  //uart_printf(UART__0, "a_gpio_producer_isr : Interrupt Occured");
  xSemaphoreGiveFromISR(switch_signal,&yield);
  LPC_GPIOINT->IO0IntClr |= (1<<29);
  portYIELD_FROM_ISR(yield);
  //uart_printf(UART__0, "a_gpio_producer_isr : Clearing Interrupt");
  //a_interrupt_clear(&inpin1);
}

static void a_task_gpio_consumer(void *params){
  IO_PORT_PIN *op_pin = (IO_PORT_PIN *) params;
  while(1){
    uart_printf(UART__0, "a_task_gpio_consumer : Waiting for Interrupt\n");
    if(xSemaphoreTake(switch_signal,(TickType_t) 10) == pdTRUE){
      my_gpio_toggle(op_pin);
      uart_printf(UART__0, "a_task_gpio_consumer : Got Interrupt");
    }
  }
}

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
*/
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

static void uart0_init(void) {
  // Note: PIN functions are initialized by board_io__initialize() for P0.2(Tx) and P0.3(Rx)
  uart__init(UART__0, clock__get_peripheral_clock_hz(), 115200);

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  QueueHandle_t tx_queue = xQueueCreate(128, sizeof(char));
  QueueHandle_t rx_queue = xQueueCreate(32, sizeof(char));
  uart__enable_queues(UART__0, tx_queue, rx_queue);
}
