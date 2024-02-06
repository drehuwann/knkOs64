#pragma once

#include "typedefs.h"
#include "io.h"
#include "klog.h"

#define PIT_CH0         0x40    /* PIT channel0 port (r/w) routed to irq 0*/
#define PIT_CH1         0x41    /* PIT channel1 port (r/w), obsolete, unused */
#define PIT_CH2         0x42    /* PIT channel2 port (r/w) routed to speaker */
#define PIT_CMD         0x43    /* PIT command port (wo) */

void Sleepd(double seconds);
void Sleep(u64 milliseconds);

void SetDivisor(u16 divisor);
u64 GetFrequency();
void SetFrequency(u64 frequency);
u16 read_pit_count(void);
void set_pit_count(u16 count);
void Tick();
double GetTimeSinceBoot();