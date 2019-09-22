#include "a_interrupt.h"


void a_enable_interrupt(A_PERIPHERAL_INTERRUPT *interrupt_config){
    lpc_peripheral__enable_interrupt(interrupt_config->peripheral,interrupt_config->peripheral_isr_callback);
    if(interrupt_config->peripheral == LPC_PERIPHERAL__GPIO){
        NVIC_EnableIRQ(GPIO_IRQn);
    }
}


void a_edge_enable_interrupt(IO_PORT_PIN *interrupt_pin, int edge){
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

void a_interrupt_init(A_PERIPHERAL_INTERRUPT *interrupt_config){
    a_enable_interrupt(interrupt_config);
    a_edge_enable_interrupt(&(interrupt_config->interrupt_pin), interrupt_config->edge);

}


void a_interrupt_create(A_PERIPHERAL_INTERRUPT *interrupt_type, lpc_peripheral_e periferal, 
                                function__void_f peripheral_isr_callback, IO_PORT_PIN *interrupt_pin,int edge){
    interrupt_type->peripheral = periferal;
    interrupt_type->peripheral_isr_callback = peripheral_isr_callback;
    interrupt_type->interrupt_pin = interrupt_pin;
    interrupt_type->edge = edge;
    //a_interrupt_init(interrupt_type);
}

void a_interrupt_clear(IO_PORT_PIN *interrupt_pin){
    if(interrupt_pin->port == LPC_GPIO0){
        LPC_GPIOINT->IO0IntClr |= (1<<interrupt_pin->pin_num);
    }
    else if(interrupt_pin->port == LPC_GPIO2){
        LPC_GPIOINT->IO2IntClr |= (1<<interrupt_pin->pin_num);
    }
}