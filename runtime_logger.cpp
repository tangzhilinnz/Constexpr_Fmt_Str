#include <stdint.h>
#include <iostream>

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


// RuntimeLogger constructor
RuntimeLogger::RuntimeLogger()
    : threadBuffers()
    , bufferMutex()
    , condMutex()
    , workAdded()
    , bgThread()
    , bgThreadShouldExit(false)
    , outputFp(nullptr)
    , currentLogLevel(LogLevel::INFORMATION)
    , logCB(nullptr)
    , maxCBLogLevel(LogLevel::WARNING)
    , minCBPeriod(0) {
    bgThread = std::thread(&RuntimeLogger::poll);
}

// RuntimeLogger destructor
RuntimeLogger::~RuntimeLogger() {
    if (outputFp) {
        fclose(outputFp);
        outputFp = nullptr;
    }

    // Stop the compression thread
    {
        std::lock_guard<std::mutex> lock(tzLogSingleton.condMutex);
        tzLogSingleton.bgThreadShouldExit = true;
        tzLogSingleton.workAdded.notify_all();
    }

    if (tzLogSingleton.bgThread.joinable())
        tzLogSingleton.bgThread.join();

    std::cout << "BG thread stopped!!!" << std::endl;
}

// See documentation in NanoLog.h
void RuntimeLogger::preallocate() {
    tzLogSingleton.ensureStagingBufferAllocated();
    // I wonder if it'll be a good idea to update minFreeSpace as well since
    // the user is already willing to invoke this up front cost.
}

void RuntimeLogger::setLogFile_internal(const char* filename) {
    FILE* newFp = fopen(filename, "a");
    if (!newFp) {
        std::string err = "Unable to open file new log file: '";
        err.append(filename);
        err.append("': ");
        err.append(strerror(errno));
        throw std::ios_base::failure(err);
    }

    if (outputFp) fclose(outputFp);
    outputFp = newFp;
}

/**
 * Set where the NanoLog should output its compressed log. If a previous
 * log file was specified, NanoLog will attempt to sync() the remaining log
 * entries before swapping files. For best practices, the output file shall
 * be set before the first invocation to log by the main thread as this
 * function is *not* thread safe.
 *
 * By default, the NanoLog will output to /tmp/compressedLog
 *
 * \param filename
 *      File for NanoLog to output the compress log
 *
 * \throw is_base::failure
 *      if the file cannot be opened or crated
 */
void RuntimeLogger::setLogFile(const char* filename) {
    tzLogSingleton.setLogFile_internal(filename);
}

/**
 * Sets the minimum log level new NANO_LOG messages will have to meet before
 * they are saved. Anything lower will be dropped.
 *
 * \param logLevel
 *      LogLevel enum that specifies the minimum log level.
 */
void RuntimeLogger::setLogLevel(LogLevel logLevel) {
    //if (logLevel < 0)
    //    logLevel = static_cast<LogLevel>(0);
    //else if (logLevel >= LogLevel::NUM_LOG_LEVELS)
    //    logLevel = static_cast<LogLevel>(LogLevel::NUM_LOG_LEVELS - 1);
    tzLogSingleton.currentLogLevel = logLevel;
}

void RuntimeLogger::poll_() {
    //char compress_buf[1024 * 1024];

    char output_buf[64 * 1024 * 1024];

    char* pBuf = output_buf;
    size_t spaceLeft = 64 * 1024 * 1024;
    size_t bytesWritten = 0;
    unsigned int count = 0;

    // Each iteration of this loop scans for uncompressed log messages in the
    // thread buffers, compresses as much as possible, and outputs it to a file.
    // The loop will run so long as it's not shutdown or there's outstanding I/O
    while (!bgThreadShouldExit) {
        bytesWritten = 0;

        {
            std::unique_lock<std::mutex> lock(bufferMutex);
            for (int i = 0; i < threadBuffers.size(); i++) {
                StagingBuffer* sb = threadBuffers[i];

                uint64_t peekBytes = 0;
                char* peekPosition = sb->peek(&peekBytes);

                // If there's work, unlock to perform it
                if (peekBytes > 0) {
                    lock.unlock();

                    if (spaceLeft <= peekBytes) {
                        pBuf = output_buf;
                        spaceLeft = 64 * 1024 * 1024;
                    }

                    memcpy(pBuf, peekPosition, peekBytes);
                    spaceLeft -= peekBytes;

                    bytesWritten += peekBytes;

                    sb->consume(peekBytes);

                    count = 0;

                    lock.lock();
                }
                else {
                    // If there's no work, check if we're supposed to delete
                    // the stagingBuffer
                    if (sb->checkCanDelete()) {
                        delete sb;

                        threadBuffers.erase(threadBuffers.begin() + i);
                        if (threadBuffers.empty()) {
                            break;
                        }

                        --i;
                    }
                }

            }
        }

        //if (outputFp) {
        //    fflush(outputFp);
        //}

        // If there's no data to output, go to sleep.
        if (bytesWritten == 0) {
            if (++count >= 10000) {
                //std::this_thread::sleep_for(std::chrono::microseconds(POLL_INTERVAL_NO_WORK_US));

                std::unique_lock<std::mutex> lock(condMutex);
                workAdded.wait_for(lock, std::chrono::microseconds(
                    POLL_INTERVAL_NO_WORK_US));
                //count = 0;
            }
        }
    }
}