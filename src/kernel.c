#include "uart.h"
#include "mb.h"
#include "rand.h"
#include "delay.h"
#include "framebuffer.h"
#include "mmu.h"

#define KERNEL_UART0_DR        ((volatile unsigned int*)0xFFFFFFFFFFE00000)
#define KERNEL_UART0_FR        ((volatile unsigned int*)0xFFFFFFFFFFE00018)

void main()
{   
    uart_init();

    fb_init();

    mmu_init();

    drawPixel(250,250,0x0e);

    drawString(10,10,"Hello world!",0x0f);
    drawString(1020, 200, "It works!!!", 0xf);

    while(1) {
        uart_putc(uart_getc());
    }

}
