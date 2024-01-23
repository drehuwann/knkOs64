#include "klog.h"

static logLvl curLvl = NONE;

void setCurrentLogLevel(logLvl lvl) {
    curLvl = lvl;
}

void klog(const u8 *str, const u8 *file, s64 line, logLvl lvl) {
    if (curLvl == NONE || lvl > curLvl) return; //do nothing
    write_serial('[');
    pushStr2Serial(lvlStr[lvl]);
    pushStr2Serial("]\t");
    pushStr2Serial(str);
    write_serial('(');
    pushStr2Serial(file);
    write_serial(':');
    pushStr2Serial(int2str(line));
    pushStr2Serial(")\n");
}
