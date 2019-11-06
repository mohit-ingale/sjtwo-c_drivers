#include "a_memory.h"

#define MEMORY_SIZE_MAX 100

static uint8_t my_memory[MEMORY_SIZE_MAX] = {0};

uint8_t a_memory_write(uint8_t memory_address, uint8_t data) {
  fprintf(stderr, "Writing to memory = %d\n", memory_address);
  if (memory_address <= MEMORY_SIZE_MAX) {
    my_memory[memory_address - 1] = data;
    return 1;
  } else {
    fprintf(stderr, "Error Writing\n");
    return 0;
  }
}

uint8_t a_memory_read(uint8_t memory_address) {
  if (memory_address <= MEMORY_SIZE_MAX) {
    return my_memory[memory_address - 1];
  } else {
    fprintf(stderr, "Error reading\n");
    return 0;
  }
}