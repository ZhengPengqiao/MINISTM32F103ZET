//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#ifndef TIMER_H_
#define TIMER_H_

#include "cmsis_device.h"

// ----------------------------------------------------------------------------

#define TIMER_MS_HZ (1000u)
#define TIMER_US_HZ (1000000u)

typedef uint32_t timer_ticks_t;

extern volatile timer_ticks_t timer_InterruptCount;

extern void timer_init (void);
extern void timer_us(u32 nus);
extern void timer_ms(u32 nms);
// ----------------------------------------------------------------------------

#endif // TIMER_H_
