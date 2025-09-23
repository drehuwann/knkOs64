#include "klog.h"

static logLvl curLvl = NONE;

void setCurrentLogLevel(logLvl lvl) {
    curLvl = lvl;
}

static const u8 *lvlStr[NONE + 1] = {
    (const u8 *)"FATAL", (const u8 *)"ERROR", (const u8 *)"WARN",
    (const u8 *)"INFO", (const u8 *)"DEBUG", (const u8 *)"TRACE",
    (const u8 *)"NONE"
};

const u8 *logLvlStr(logLvl lvl) {
    if (lvl < 0 || lvl > NONE) return (const u8 *)"UNKNOWN";
    return lvlStr[lvl];
}

void klog(const u8 *str, const u8 *file, s64 line, logLvl lvl) {
    if (curLvl == NONE || lvl > curLvl) return; //do nothing
    write_serial('[');
    pushStr2Serial(lvlStr[lvl]);
    pushStr2Serial((const u8 *)"]\t");
    pushStr2Serial(str);
    write_serial('(');
    pushStr2Serial(file);
    write_serial(':');
    pushStr2Serial(int2str(line));
    pushStr2Serial((const u8 *)")\n");
}
