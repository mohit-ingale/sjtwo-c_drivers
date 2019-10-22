#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "a_uart.h"
#include "delay.h"
#include "queue.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"

static void a_uart_task_tx(void *params);
static void a_uart_task_rx(void *params);
static void a_uart_task_rx_from_queue(void *params);
void a_uart_rx_isr();

QueueHandle_t xQueueRead;
SemaphoreHandle_t sem_handle;

int main(void) {
  a_uart_init(UART3, 9600);
  // a_uart_rx_enable(UART3, a_uart_rx_isr);
  sem_handle = xSemaphoreCreateMutex();

  xQueueRead = xQueueCreate(16, sizeof(uint8_t));
  if (xQueueRead == NULL) {
    printf("Queue not created\n");
  }

  xTaskCreate(a_uart_task_tx, "uart_task_tx", (4096U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(a_uart_task_rx, "uart_task_rx", (4096U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(a_uart_task_rx_from_queue, "uart_task_rx_queue", (4096U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  sj2_cli__init();
  // UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
  // puts("Starting RTOS\n");
  vTaskStartScheduler();
  return 0;
}

void a_uart_rx_isr() {
  uint8_t data;
  int taskgive;
  fprintf(stderr, "Why i got interrupted = %d\n", a_uart_interrupt_which(UART3) >> 1);
  a_uart_rx(UART3, &data);
  xQueueSendFromISR(xQueueRead, &data, &taskgive);
  // if (taskgive) {
  //   taskYIELD();
  // }
}

static void a_uart_task_tx(void *params) {
  int data;
  char s_data[16] = {0};
  int i = 0;
  while (1) {
    // printf("Transmitting The character\n");
    data = rand();
    sprintf(s_data, "%i", data);
    printf("Data to be transmitted = %s\n", s_data);
    for (i = 0; i < strlen(s_data); i++) {
      a_uart_tx(UART3, (uint8_t)s_data[i]);
      // vTaskDelay(10);
    }
    a_uart_tx(UART3, '\0');
    for (i = 0; i < strlen(s_data); i++) {
      s_data[i] = 0;
    }
    vTaskDelay(1000);
  }
}

static void a_uart_task_rx(void *params) {
  static char data[16] = {0};
  char temp_data;
  static int i = 0;
  while (1) {
    // printf("Receving The character\n");
    a_uart_rx(UART3, &temp_data);
    if ((temp_data != '\0') && (i < 15)) {
      data[i++] = temp_data;
    } else {
      data[i++] = '\0';
      i = 0;
      printf("Received Data = %s\n", data);
    }

    // vTaskDelay(100);
  }
}

static void a_uart_task_rx_from_queue(void *params) {
  char data[16] = {0};
  char temp_data;
  static int i = 0;
  while (1) {
    while (!(xQueueReceive(xQueueRead, &(temp_data), (TickType_t)10))) {
    }
    printf("Received Data from queue= %c\n", temp_data);

    // printf("Received Data from queue= %c\n", temp_data);
    vTaskDelay(100);
  }
}

/*
------------------NOT NEEDED FOR THIS ASSIGNMENT-----------------------
*/
/*

static void a_verify_adesco_signature() {
  static STATUS_REGISTER_ADESTO a_status_register_flash_memory;
  EXTERNAL_FLASH_SIGNATURE a_external_device_signature;
  while (1) {
    if (xSemaphoreTake(xMutex, (TickType_t)10) == pdTRUE) {
      a_status_register_flash_memory = a_ssp_read_device_status();
      a_external_device_signature = a_ssp_read_device_signature();
      if (a_external_device_signature.manufacturer_id != 0x1F) {
        fprintf(stderr, "Manufacturer ID read failure\n");
        vTaskSuspend(NULL); // Kill this task
      }
      xSemaphoreGive(xMutex);
      // printf("Manufacture ID = %x\n", a_external_device_signature.manufacturer_id);
      // printf("Device ID = %x\n", a_external_device_signature.device_id_1);
      // printf("Device ID = %x\n", a_external_device_signature.device_id_2);
      // printf("Extended Device Data = %x\n", a_external_device_signature.extended_device_information);
      // print_status(&a_status_register_flash_memory);
    }
    vTaskDelay(500);
  }
}

static void a_task_spi(void *params) {
  EXTERNAL_FLASH_SIGNATURE a_external_device_signature;
  STATUS_REGISTER_ADESTO a_status_register_flash_memory;
  uint8_t data = 0;
  while (1) {
    // a_status_register_flash_memory = a_ssp_read_device_status();
    a_external_device_signature = a_ssp_read_device_signature();
    printf("Manufacture ID = %x\n", a_external_device_signature.manufacturer_id);
    printf("Device ID = %x\n", a_external_device_signature.device_id_1);
    printf("Device ID = %x\n", a_external_device_signature.device_id_2);
    printf("Extended Device Data = %x\n", a_external_device_signature.extended_device_information);
    vTaskDelay(1000);
  }
}


static void a_task_pwm_run(void *params){
  uint16_t dutycycle = 0;
  const uint32_t config_mask = UINT32_C(7);
  while(1){
    if(xQueueReceive(r_pwm_duty_cycle_queue,&dutycycle,100)){
      // uart_printf(UART__0,"Received_DATA = %d\n",dutycycle);
      a_pwm_set_duty_cycle(0,dutycycle);
    }
    if(xQueueReceive(g_pwm_duty_cycle_queue,&dutycycle,100)){
      // uart_printf(UART__0,"Received_DATA = %d\n",dutycycle);
      a_pwm_set_duty_cycle(1,dutycycle);
    }
    if(xQueueReceive(b_pwm_duty_cycle_queue,&dutycycle,100)){
      // uart_printf(UART__0,"Received_DATA = %d\n",dutycycle);
      a_pwm_set_duty_cycle(2,dutycycle);
    }
  }
}

static void a_task_adc_run(void *params){
  uint16_t l_a_adc_data;
  uint16_t dutycycle = 50;
  float adc_voltage = 0.0;
  a_adc_init(1);
  while(1){
    a_adc_start(2);
    l_a_adc_data = a_get_adc_data(2);
    dutycycle = (l_a_adc_data * 100)/4095;
    adc_voltage = ((float)l_a_adc_data/4095)*3.3;
    printf("ADC_DATA = %f\n",adc_voltage);
    xQueueSend(r_pwm_duty_cycle_queue,(void *)&dutycycle,100);
     a_adc_start(4);
    l_a_adc_data = a_get_adc_data(4);
    dutycycle = (l_a_adc_data * 100)/4095;
    adc_voltage = (l_a_adc_data * 3.3)/4095;
    printf("ADC_DATA = %f\n",adc_voltage);
    xQueueSend(g_pwm_duty_cycle_queue,(void *)&dutycycle,100);
     a_adc_start(5);
    l_a_adc_data = a_get_adc_data(5);
    dutycycle = (l_a_adc_data * 100)/4095;
    adc_voltage = (l_a_adc_data * 3.3)/4095;
    printf("ADC_DATA = %f\n",adc_voltage);
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
