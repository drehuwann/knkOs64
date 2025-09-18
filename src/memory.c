#include "memory.h"

void memset(void *start, u64 value, u64 num) {
    if (num <= 8) {
        u8 const *valPtr = (u8 *)&value;
        for (u8 * ptr = (u8 *)start; ptr < (u8 *)((u64)start + num); ptr ++) {
            *ptr = *valPtr ++;
        }
        return;
    }
    u64 proceedingBytes = num % 8;
    u64 newnum = num - proceedingBytes;
    for (u64 *ptr = (u64 *)start; ptr < (u64 *)((u64)start + newnum); ptr++) {
        *ptr = value;
    }
    u8 const* valPtr = (u8*)&value;
    for (u8* ptr = (u8 *)((u64)start + newnum); ptr < (u8 *)((u64)start + num); ptr ++) {
        *ptr = *valPtr ++;
    }
}

void memcpy(void *dest, void *src, u64 num) {
    if (num <= 8) {
        u8 const *valPtr = (u8 *)src;
        for (u8 * ptr = (u8 *)dest; ptr < (u8 *)((u64)dest + num); ptr ++) {
            *ptr = *valPtr ++;
        }
        return;
    }
    u64 proceedingBytes = num % 8;
    u64 newnum = num - proceedingBytes;
    u64 const *srcptr = (u64 *)src;
    for (u64 *destptr = (u64 *)dest; destptr < (u64 *)((u64)dest + newnum); destptr++) {
        *destptr = *srcptr ++;
    }
    u8 const* srcptr8 = (u8*)((u64)src + newnum);
    for (u8* destptr8 = (u8 *)((u64)dest + newnum); destptr8 < (u8 *)((u64)dest + num); destptr8 ++) {
        *destptr8 = *srcptr8 ++;
    }
}
