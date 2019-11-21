
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdlib.h>

#include "a_i2c.h"
#include "a_memory.h"
#include "acceleration.h"
#include "delay.h"
#include "sj2_cli.h"
#include "task.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)

static void a_i2c_slave_task(void *params);

int main(void) {
  // my_memory = calloc(MEMORY_SIZE_MAX, sizeof(uint8_t));
  // if(my_memory==NULL){
  //   printf("Error Allocating Memory\n");
  // }
  a_acclerometer_signal = xSemaphoreCreateBinary();
  a_i2c_slave_init(a_i2c_bus_1);

  // xTaskCreate(a_producer, "a_producer", (4096U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(a_consumer, "a_consumer", (4096U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(a_i2c_slave_task, "a_i2c_slave_task", (4096U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);

  sj2_cli__init();
  // UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
  // puts("Starting RTOS\n");
  vTaskStartScheduler();
  return 0;
}

static void a_i2c_slave_task(void *params) {
  while (1) {
    if (xSemaphoreTake(a_acclerometer_signal, portMAX_DELAY)) {
      printf("Accl Init\n");
      acceleration__init();
    }
  }
}
