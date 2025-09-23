#include "pit.h"

static double TimeSinceBoot = 0;
static u16 Divisor = 65535;
static const u64 BaseFrequency = 1193182;

/**
  * Bits         Usage
    6 and 7      Select channel :
                    0 0 = Channel 0
                    0 1 = Channel 1
                    1 0 = Channel 2
                    1 1 = Read-back command (8254 only)
    4 and 5      Access mode :
                    0 0 = Latch count value command
                    0 1 = Access mode: lobyte only
                    1 0 = Access mode: hibyte only
                    1 1 = Access mode: lobyte/hibyte
    1 to 3       Operating mode :
                    0 0 0 = Mode 0 (interrupt on terminal count)
                    0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                    0 1 0 = Mode 2 (rate generator)
                    0 1 1 = Mode 3 (square wave generator)
                    1 0 0 = Mode 4 (software triggered strobe)
                    1 0 1 = Mode 5 (hardware triggered strobe)
                    1 1 0 = Mode 2 (rate generator, same as 010b)
                    1 1 1 = Mode 3 (square wave generator, same as 011b)
    0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
static bool init_pit() { //mode0, ch0, lo/hi, binary
    klog((const u8 *)"in 'bool init_pit()'", (const u8 *)__FILE__, __LINE__, TRACE);
    static bool done = false;
    if (! done) {
        outb(PIT_CMD, 0b00110000);
        done = true;
    }
    return done;
}

void Sleepd(double seconds) {
    double startTime = TimeSinceBoot;
    while (TimeSinceBoot < startTime + seconds){
        asm volatile("hlt");
    }
}

void Sleep(u64 milliseconds) {
    Sleepd((double)milliseconds / 1000);
}

void SetDivisor(u16 divisor){
    klog((const u8 *)"in 'void SetDivisor(u16)'", (const u8 *)__FILE__, __LINE__, TRACE);
    if (divisor < 100) divisor = 100;
    Divisor = divisor;
    outb(PIT_CH0, (u8)(divisor & 0x00ff));
    outb(PIT_CH0, (u8)((divisor & 0xff00) >> 8));
}

u64 GetFrequency() {
    return BaseFrequency / Divisor;
}

void SetFrequency(u64 frequency) {
    klog((const u8 *)"in 'void SetFrequency(u64)'", (const u8 *)__FILE__, __LINE__, TRACE);
    if (init_pit()) SetDivisor(BaseFrequency / frequency);
}

u16 read_pit_count(void) {
	u16 count = 0;
 
	// Disable interrupts
	asm volatile ("cli");
 
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_CMD, 0b0000000);
 
	count = inb(PIT_CH0);		// Low byte
	count |= inb(PIT_CH0) << 8;		// High byte 
	return count;
}

void set_pit_count(u16 count) {
	// Disable interrupts
	asm volatile("cli");
 
	// Set low byte
	outb(PIT_CH0, (u8)(count & 0xff));		// Low byte
	outb(PIT_CH0, (u8)((count & 0xff00) >> 8));	// High byte
}

void Tick() {
    u64 freq = GetFrequency();
    SetFrequency(freq);
    TimeSinceBoot += 1 / (double)freq;
}

double GetTimeSinceBoot() {
    return TimeSinceBoot;
}
