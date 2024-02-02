#pragma once

#include "typedefs.h"
#include "io.h"
#include "klog.h"

extern double TimeSinceBoot;

void Sleepd(double seconds);
void Sleep(u64 milliseconds);

void SetDivisor(u16 divisor);
u64 GetFrequency();
void SetFrequency(u64 frequency);
void Tick();
