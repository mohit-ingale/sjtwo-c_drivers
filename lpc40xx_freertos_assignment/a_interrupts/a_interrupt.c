#include "a_interrupt.h"

static function__void_f a_gpio_callback[32];

static void a_enable_interrupt(A_PERIPHERAL_INTERRUPT *interrupt_config){
    lpc_peripheral__enable_interrupt(interrupt_config->peripheral,interrupt_config->peripheral_isr_callback);
    if(interrupt_config->peripheral == LPC_PERIPHERAL__GPIO){
        NVIC_EnableIRQ(GPIO_IRQn);
    }
}
static int isPowerOfTwo(uint32_t n) 
{ 
    return n && (!(n & (n - 1))); 
}

static uint32_t findPosition(uint32_t n) 
{ 
    if (!isPowerOfTwo(n)) 
        return 0;
    unsigned i = 1, pos = 1;
    while (!(i & n)) { 
        i = i << 1; 
        ++pos;
    } 
  
    return pos; 
} 

static void a_interrupt_dispatcher(void){
    uint32_t interrupt_pin=0;
    if(interrupt_pin = findPosition((LPC_GPIOINT->IO0IntStatF) & 0xffffffff)){
        a_gpio_callback[interrupt_pin-1]();
        LPC_GPIOINT->IO0IntClr |= (1<<(interrupt_pin-1));
    }
    else if(interrupt_pin = findPosition((LPC_GPIOINT->IO0IntStatR) & 0xffffffff)){
        a_gpio_callback[interrupt_pin-1]();
        LPC_GPIOINT->IO0IntClr |= (1<<(interrupt_pin-1));
    }
    else if(interrupt_pin = findPosition((LPC_GPIOINT->IO2IntStatR) & 0xffffffff)){
        a_gpio_callback[interrupt_pin-1]();
        LPC_GPIOINT->IO2IntClr |= (1<<(interrupt_pin-1));
    }
    else if(interrupt_pin = findPosition((LPC_GPIOINT->IO2IntStatF) & 0xffffffff)){
        a_gpio_callback[interrupt_pin-1]();
        LPC_GPIOINT->IO2IntClr |= (1<<(interrupt_pin-1));
    }
}

static void a_edge_enable_interrupt(IO_PORT_PIN *interrupt_pin, int edge){
    if((edge==RISING_EDGE) && (interrupt_pin->port == LPC_GPIO0)){
        LPC_GPIOINT->IO0IntEnR |= (1<<interrupt_pin->pin_num);
    }
    else if((edge==RISING_EDGE) && (interrupt_pin->port == LPC_GPIO2)){
        LPC_GPIOINT->IO2IntEnR |= (1<<interrupt_pin->pin_num);
    }
    else if((edge==FALLING_EDGE) && (interrupt_pin->port == LPC_GPIO0)){
        LPC_GPIOINT->IO0IntEnF |= (1<<interrupt_pin->pin_num);
    }
    else if((edge==FALLING_EDGE) && (interrupt_pin->port == LPC_GPIO2)){
        LPC_GPIOINT->IO2IntEnF |= (1<<interrupt_pin->pin_num);
    }
}

static void a_interrupt_init(A_PERIPHERAL_INTERRUPT *interrupt_config){
    a_enable_interrupt(interrupt_config);
    a_edge_enable_interrupt(interrupt_config->interrupt_pin, interrupt_config->edge);
}


void a_interrupt_create(A_PERIPHERAL_INTERRUPT *interrupt_type, lpc_peripheral_e periferal, 
                                function__void_f peripheral_isr_callback, IO_PORT_PIN *interrupt_pin,int edge){
    a_gpio_callback[interrupt_pin->pin_num] = peripheral_isr_callback;
    interrupt_type->peripheral = periferal;
    interrupt_type->peripheral_isr_callback = a_interrupt_dispatcher;
    interrupt_type->interrupt_pin = interrupt_pin;
    interrupt_type->edge = edge;
    a_interrupt_init(interrupt_type);
}

void a_interrupt_clear(IO_PORT_PIN *interrupt_pin){
    if(interrupt_pin->port == LPC_GPIO0){
        LPC_GPIOINT->IO0IntClr |= (1<<interrupt_pin->pin_num);
    }
    else if(interrupt_pin->port == LPC_GPIO2){
        LPC_GPIOINT->IO2IntClr |= (1<<interrupt_pin->pin_num);
    }
}