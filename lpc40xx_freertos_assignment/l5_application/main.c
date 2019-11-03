

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "acceleration.h"
#include "delay.h"
#include "event_groups.h"
#include "ff.h"
#include "my_gpio.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"
#include <string.h>

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)

static void a_producer(void *params);
static void a_consumer(void *params);
static void watchdog_task(void *params);

bool a_file_open(char *filename, FIL *file);
void a_file_close(FIL *file);
bool a_file_write(FIL *file, char *string);

QueueHandle_t xQueueSensor;

EventGroupHandle_t watch_dog;

int main(void) {
  xQueueSensor = xQueueCreate(20, sizeof(acceleration__axis_data_s));
  if (xQueueSensor == NULL) {
    printf("Queue not created\n");
  }

  watch_dog = xEventGroupCreate();

  // xTaskCreate(a_producer, "a_producer", (4096U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(a_consumer, "a_consumer", (4096U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(watchdog_task, "watchdog_task", (4096U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

  sj2_cli__init();
  // UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
  // puts("Starting RTOS\n");
  vTaskStartScheduler();
  return 0;
}

static void a_producer(void *params) {
  static int i = 0;
  static acceleration__axis_data_s avg_data = {0};
  acceleration__axis_data_s data;
  acceleration__init();
  while (1) {
    data = acceleration__get_data();
    i++;
    if (i <= 100) {
      avg_data.x += data.x;
      avg_data.y += data.y;
      avg_data.z += data.z;
    } else {
      avg_data.x /= 100;
      avg_data.y /= 100;
      avg_data.z /= 100;
      while (!xQueueSend(xQueueSensor, &avg_data, portMAX_DELAY))
        ;
      xEventGroupSetBits(watch_dog, BIT_0);
      vTaskDelay(1);

      avg_data.x = 0;
      avg_data.y = 0;
      avg_data.z = 0;
      i = 0;
    }

    vTaskDelay(100);
  }
}

static void a_consumer(void *params) {
  acceleration__axis_data_s avg_data;
  TickType_t myticks = xTaskGetTickCount();
  FIL file;
  const char *filename = "sensor.txt";
  a_file_open(filename, &file);
  while (1) {
    // while (xQueueReceive(xQueueSensor, &avg_data, 10) != pdTRUE)
    //   ;
    if (xQueueReceive(xQueueSensor, &avg_data, 10) == pdTRUE) {
      char string[64];
      // printf("time = %i\n", xTaskGetTickCount());
      sprintf(string, "%i,%i,%i,%i\n", xTaskGetTickCount(), avg_data.x, avg_data.y, avg_data.z);
      a_file_write(&file, string);
      // printf("Data_x = %d, Data_y= %d, Data_z= %d\n", avg_data.x, avg_data.y, avg_data.z);
      if (xTaskGetTickCount() - myticks > 100000) {
        a_file_close(&file);
        if (!a_file_open(filename, &file)) {
          printf("File not able to open\n");
        }
        myticks = xTaskGetTickCount();
      }

      // write_file_using_fatfs_pi(avg_data);
    }
    xEventGroupSetBits(watch_dog, BIT_1);
    vTaskDelay(1);
  }
}

bool a_file_open(char *filename, FIL *file) {

  FRESULT result = f_open(file, filename, (FA_WRITE | FA_OPEN_APPEND));
  if (FR_OK == result) {
    return true;
  } else {
    return false;
  }
}

void a_file_close(FIL *file) { f_close(file); }

bool a_file_write(FIL *file, char *string) {
  UINT bytes_written = 0;
  if (FR_OK == f_write(file, string, strlen(string), &bytes_written)) {
  } else {
    printf("ERROR: Failed to write data to file\n");
  }
  f_sync(file);
}

static void watchdog_task(void *params) {
  EventBits_t uxBits;
  FIL file;
  const char *filename = "log.txt";

  while (1) {
    vTaskDelay(10000);
    uxBits = xEventGroupWaitBits(watch_dog,     /* The event group being tested. */
                                 BIT_0 | BIT_1, /* The bits within the event group to wait for. */
                                 pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
                                 pdTRUE,        /* Don't wait for both bits, either bit will do. */
                                 1000);
    if ((uxBits & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1)) {
    } else if ((uxBits & BIT_0) != 0) {
      a_file_open(filename, &file);
      a_file_write(&file, "Producer Failed\n");
      printf("one bits set\n");
      a_file_close(&file);
    } else if ((uxBits & BIT_1) != 0) {
      a_file_open(filename, &file);
      a_file_write(&file, "Consumer Failed\n");
      printf("one bits set\n");
      a_file_close(&file);
    } else {
      a_file_open(filename, &file);
      a_file_write(&file, "Producer & Consumer Failed\n");
      printf("none bits set\n");
      a_file_close(&file);
    }
  }
}
