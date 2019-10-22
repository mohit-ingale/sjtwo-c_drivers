
/*
1. when both task are of equal priority
  a. producer sends the switch value and receiver receivers the value and prints it.
  b. when producer is not sending the value, the receiver is waiting to recevie anything from queue
  c. When producer sends the data to queue, when the reciver task is scheduled to run, it will print the value
      received from the queue
  d. We can see in output that the receiver is waiting for data in queue, Producer sends the data in the queue then
receiver reads the data from the queue
2. When Producer is of higher priority and consumer is of lower priority
  a. producer sends the switch value and receiver is waiting for the data to arrive in queue
    producer sleeps after sending the value, then consumer task reads the data from the queue and prints it.
  b. when producer is not sending the value, the receiver is not receving anything from queue it is waiting for data
    to arrive in the queue
    The output is similar to equal priority as consumer is of low priority it will wait for data and read data from
queue when high priority producer has completed everything
3. When Consumer is of Higher priority than producer
    a. Consumer will be waiting on the queue and will sleep for 10 ticks
    b. When producer will come into ready state and consumer is sleeping the producer will read switch and put in queue
    c. Immediately context switch will happen and then consumer task will wake up and read data from queue and print the
      output
    d. Then again consumer will sleep waiting for data on queue and producer will complete its remaining task of
      printing data is sent



Additional Question
when wait ticks is equal to 0 in receive queue
when consumer is of high priority
  a. Consumer will keep on waiting on the queue doing busy looping and consume cpu indefinitetly
  b. Producer will never get cpu to execute the code and send data to the queue, hence receiver would never get data

Purpose of block time
  a. If data is not available in queue the task would sleep and wait till either timeout occurs or data is available
     in the queue


*/

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "delay.h"
#include "my_gpio.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"
typedef enum { switch__off, switch__on } switch_e;

static void a_producer(void *params);
static void a_consumer(void *params);

QueueHandle_t xQueueProducer;
static struct IO_PORT_PIN a_switch_for_queue;
int main(void) {
  my_gpio_init(0, 29, IN, &a_switch_for_queue);
  xQueueProducer = xQueueCreate(1, sizeof(switch_e));
  if (xQueueProducer == NULL) {
    printf("Queue not created\n");
  }

  xTaskCreate(a_producer, "a_producer", (4096U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(a_consumer, "a_consumer", (4096U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  sj2_cli__init();
  // UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
  // puts("Starting RTOS\n");
  vTaskStartScheduler();
  return 0;
}

static void a_producer(void *params) {
  switch_e data;
  while (1) {
    data = (switch_e)my_gpio_get(&a_switch_for_queue);
    printf("Data Sent Before = %i\n", data);
    xQueueSend(xQueueProducer, (void *)&data, 0);
    printf("Data Sent After = %i\n", data);
    vTaskDelay(1000);
  }
}

static void a_consumer(void *params) {
  switch_e data;
  while (1) {
    // while (xQueueReceive(xQueueProducer, &data, 10) != pdTRUE)
    //   ;
    printf("Data receive before = %i\n", data);
    xQueueReceive(xQueueProducer, &data, portMAX_DELAY);
    printf("Data receive after = %i\n", data);
    // vTaskDelay(1000);
  }
}
