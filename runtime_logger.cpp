#include <stdint.h>

#include "runtime_logger.h"

TSCNS tscns;
uint64_t midnight_ns;

class TimeIniter {
public:
    TimeIniter() {
        tscns.init();
        time_t rawtime = tscns.rdns() / 1000000000;
        struct tm* timeinfo = localtime(&rawtime);
        timeinfo->tm_sec = timeinfo->tm_min = timeinfo->tm_hour = 0;
        midnight_ns = mktime(timeinfo) * 1000000000;
    }
};

static TimeIniter _;