#include "uart.h"
#include "mb.h"
#include "rand.h"

void main()
{   
    uart_init();

    uart_puts("Hello World!\n");

    rand_init();
    
    uart_puts("Here goes a random number: ");
    uart_hex(rand(0,4294967295));
    uart_puts("\n");
    
    while (1)
		uart_putc(uart_getc());
}
