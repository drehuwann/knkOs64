#pragma once

#include "serial.h"

/**
 * @brief indexes of logLevels.
 * logLvl::NONE keeps track of number of different loglevels implemented
*/
typedef enum{FATAL, ERROR, WARN, INFO, DEBUG, TRACE, NONE} logLvl;
static const u8 *lvlStr[NONE] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

void setCurrentLogLevel(logLvl lvl);
void klog(const u8 *str, const u8 *file, s64 line, logLvl lvl);
