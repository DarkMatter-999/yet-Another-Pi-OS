#include "uart.h"

void main()
{   
    uart_init(3);
    
    uart_puts("Hello World!\n");
    
    while (1)
		uart_putc(uart_getc());
}
