#ifndef a_interrupt
#define a_interrupt

#define FALLING_EDGE 0
#define RISING_EDGE 1

#define GPIO_PERIFERAL = LPC_PERIPHERAL__GPIO;

#include "lpc_peripherals.h"
#include "my_gpio.h"


extern void a_gpio_isr(void);

typedef struct a_peripheral_interrupt{
    lpc_peripheral_e peripheral;
    function__void_f peripheral_isr_callback;
    IO_PORT_PIN *interrupt_pin;
    int edge;
}A_PERIPHERAL_INTERRUPT;


void a_interrupt_init(A_PERIPHERAL_INTERRUPT *interrupt_config);
void a_interrupt_create(A_PERIPHERAL_INTERRUPT *interrupt_type, lpc_peripheral_e periferal, 
                                function__void_f peripheral_isr_callback, IO_PORT_PIN *interrupt_pin,int edge);

void a_interrupt_clear(IO_PORT_PIN *interrupt_pin);

#endif

