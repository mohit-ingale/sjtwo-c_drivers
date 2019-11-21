
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
  uint8_t is_init = 0;
  acceleration__axis_data_s data;
  while (1) {
    if (xSemaphoreTake(a_acclerometer_signal, 50)) {
      printf("Accl Init\n");
      acceleration__init();
      is_init = 1;
    }
    if (is_init) {
      data = acceleration__get_data();
      printf("Data = %x\n", data.x);
      my_memory[MEMORY_SIZE_MAX - 6] = data.x & 0xff;
      my_memory[MEMORY_SIZE_MAX - 5] = data.x >> 8;
      my_memory[MEMORY_SIZE_MAX - 4] = data.y;
      my_memory[MEMORY_SIZE_MAX - 3] = data.y >> 8;
      my_memory[MEMORY_SIZE_MAX - 2] = data.z;
      my_memory[MEMORY_SIZE_MAX - 1] = data.z >> 8;
    }
    vTaskDelay(1000);
  }
}
