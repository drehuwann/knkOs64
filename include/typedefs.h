#pragma once

#define _BP_ asm volatile ("xchg %bx, %bx");

#define MAXMEMREGIONS 10

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
