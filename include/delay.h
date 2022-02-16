#pragma once
#include "gpio.h"

#define SYSTMR_LO        ((volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI        ((volatile unsigned int*)(MMIO_BASE+0x00003008))

void wait_cycles(unsigned int n);

void wait_msec(unsigned int n);

unsigned long get_system_timer();

void wait_msec_st(unsigned int n);