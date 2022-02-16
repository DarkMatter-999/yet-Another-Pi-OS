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

    uart_puts("Writing through identity mapped MMIO.\n");
    char *s="Writing through MMIO mapped in higher half!\r\n";
    // test mapping
    while(*s) {
        /* wait until we can send */
        do{asm volatile("nop");}while(*KERNEL_UART0_FR&0x20);
        /* write the character to the buffer */
        *KERNEL_UART0_DR=*s++;
    }


    while(1) {
        uart_putc(uart_getc());
    }

}
