#ifndef MAILBOX_H
#define MAILBOX_H

#include <stddef.h>
#include <stdint.h>

void delay(int32_t count);

void mmio_init(int raspi);
 
void mmio_write(uint32_t reg, uint32_t data);

uint32_t mmio_read(uint32_t reg);

#endif /* MAILBOX_H */