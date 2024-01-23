#pragma once

#include "typedefs.h"
#include "io.h"
#include "textprint.h"

extern u16 com1;
extern u16 com2;
extern u16 com3;
extern u16 com4;

typedef u16 comList[4];

void printComPorts();
bool initComPort(const u8 port);

bool serial_received();
u8 read_serial();
bool is_transmit_empty();
void write_serial(u8 a);

void pushStr2Serial(const u8 *str);
