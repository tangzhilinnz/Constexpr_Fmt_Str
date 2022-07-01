#include <stdint.h>

#include "runtime_logger.h"


// Define the static members of RuntimeLogger here
ThreadLocal RuntimeLogger::StagingBuffer* RuntimeLogger::stagingBuffer = nullptr;
thread_local RuntimeLogger::StagingBufferDestroyer RuntimeLogger::sbc;
RuntimeLogger RuntimeLogger::tzLogSingleton;

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