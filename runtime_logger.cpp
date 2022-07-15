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

RuntimeLogger::SinkLogger::SinkLogger(const std::string& basename,
                                      //off_t rollSize,
                                      int flushInterval)
    : outputFp_(nullptr)
    , flushInterval_(flushInterval)
    , running_(false)
    , basename_(basename)
    //, rollSize_(rollSize)
    , thread_()
    //, latch_(1)
    , mutex_()
    , cond_()
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
    , buffers_() {

    outputFp_ = fopen(basename.c_str(), "a");
    if (!outputFp_) {
        std::string err = "Unable to open file new log file: '";
        err.append(basename);
        err.append("': ");
        err.append(strerror(errno));
        throw std::ios_base::failure(err);
    }

    running_ = true;
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
    thread_ = std::thread(std::bind(&SinkLogger::threadFunc, this));
}

void RuntimeLogger::SinkLogger::append(const char* logline, int len) {
    //std::cout << "enter append" << std::endl;

    std::lock_guard<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logline, len);
    }
    else {
        buffers_.push_back(std::move(currentBuffer_));

        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else {
            currentBuffer_.reset(new Buffer); // Rarely happens
        }
        currentBuffer_->append(logline, len);
        cond_.notify_all();
    }
}

void RuntimeLogger::SinkLogger::appendNibble(const char* logline, int len,
                                             StagingBuffer* sb) {

    auto append = [&]() {
        ThreadCheckPoint* pTCP = 
            reinterpret_cast<ThreadCheckPoint*>(currentBuffer_->current());
        std::strcpy(pTCP->name_, sb->getName());
        pTCP->blockSize_ = len;
        currentBuffer_->add(sizeof(ThreadCheckPoint));

        uint64_t remaining = len;
        while (remaining > 0) {
            auto bytesToCopy =
                (static_cast<uint64_t>(RELEASE_THRESHOLD)
                    < remaining) ? RELEASE_THRESHOLD : remaining;
            currentBuffer_->append(logline, bytesToCopy);
            logline += bytesToCopy;
            remaining -= bytesToCopy;
            sb->consume(bytesToCopy);
        } 
    };

    std::lock_guard<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > (len + sizeof(ThreadCheckPoint))) {
        append();
    }
    else {
        buffers_.push_back(std::move(currentBuffer_));

        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else {
            currentBuffer_.reset(new Buffer); // Rarely happens
        }
        append();
        cond_.notify_all();
    }
}

//void SinkLogger::writeLog(OutbufArg& fmtBuf, const char* logline, 
//                          const size_t len) {
//    static const char* logLevelNames[] = { "NAN", "ERR", "WRN", "INF", "DBG" };
//    size_t remaining = len;
//    const char* pos = logline;
//    while (remaining > 0) {
//        fmtBuf.reset();
//        //int res;
//        const OneLogEntry* pOE = reinterpret_cast<const OneLogEntry*>(pos);
//        const StaticFmtInfo* pSMI = pOE->fmtId;
//        uint64_t ns = tscns.tsc2ns(pOE->timestamp);
//        uint64_t t = (ns - midnight_ns) / 1000;
//        uint32_t us = t % 1000000;
//        t /= 1000000;
//        uint32_t s = t % 60;
//        t /= 60;
//        uint32_t m = t % 60;
//        t /= 60;
//        uint32_t h = t % 24;
//
//        const char* logLevel = 
//            logLevelNames[static_cast<size_t>(pSMI->severity_)];
//
//        //pos += sizeof(OneLogEntry);
//
//        CFMT_STR_OUTBUFARG(fmtBuf, "%02d:%02d:%02d.%06d %s:%d %s: ",
//                           h, m, s, us, pSMI->filename_, pSMI->lineNum_,
//                           logLevel);
//        pSMI->convertFN_(fmtBuf, (pos + sizeof OneLogEntry));
//        CFMT_STR_OUTBUFARG(fmtBuf, "\n");
//
//        fwrite(fmtBuf.getBufPtr(), 1, fmtBuf.getWrittenNum(), outputFp);
//
//        pos += pOE->entrySize;
//        remaining -= static_cast<size_t>(pOE->entrySize);
//    }
//}

void RuntimeLogger::SinkLogger::threadFunc() {
    std::cout << "enter threadFunc" << std::endl;

    assert(running_ == true);
    //latch_.countDown();
    //LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    char* cahce = new char[1024 * 1024];
    OutbufArg fmtBuf(cahce, 1024 * 1024);
    static const char* logLevelNames[] = { "NAN", "ERR", "WRN", "INF", "DBG" };

    while (running_) {
        //std::cout << "enter sink thread" << std::endl;

        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        std::cv_status status = std::cv_status::no_timeout;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty()) { // unusual usage!
                status = cond_.wait_for(
                    lock, std::chrono::seconds(flushInterval_));
            }
            if (std::cv_status::no_timeout == status) {
                if (currentBuffer_->length() > 0.8 * currentBuffer_->bufferSize()) {
                    buffers_.push_back(std::move(currentBuffer_));
                    currentBuffer_ = std::move(newBuffer1);
                }
            }
            else { // std::cv_status::timeout == status
                if (currentBuffer_->length() > 0) {
                    buffers_.push_back(std::move(currentBuffer_));
                    currentBuffer_ = std::move(newBuffer1);
                }
            }

            //buffers_.push_back(std::move(currentBuffer_));
            //currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        //std::cout << buffersToWrite.size() << std::endl;

        if (buffersToWrite.empty()) continue;

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {
            //char buf[256];
            //snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
            //    Timestamp::now().toFormattedString().c_str(),
            //    buffersToWrite.size() - 2);
            //fputs(buf, stderr);
            //output.append(buf, static_cast<int>(strlen(buf)));
            //buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (const auto& buffer : buffersToWrite) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            // output.append(buffer->data(), buffer->length());

            //for (int i = 0; i < 24; i++)
            //    std::cout << "";

            //writeLog(fmtBuf, buffer->data(), buffer->length());
             
            int bufLen = buffer->length();
            const char* pos = buffer->data();
            while (bufLen > 0) {
                const ThreadCheckPoint* pTCP = 
                    reinterpret_cast<const ThreadCheckPoint*>(pos);

                int remaining = pTCP->blockSize_;
                pos += sizeof(ThreadCheckPoint);

                while (remaining > 0) {

                    fmtBuf.reset();
                    //int res;
                    const OneLogEntry* pOE = 
                        reinterpret_cast<const OneLogEntry*>(pos);
                    const StaticFmtInfo* pSMI = pOE->fmtId;
                    uint64_t ns = tscns.tsc2ns(pOE->timestamp);
                    std::time_t timer = static_cast<time_t>(ns / 1000000000);
                    std::tm* now = std::localtime(&timer);
                    uint64_t t = (ns - midnight_ns) / 1000;
                    uint32_t us = t % 1000000;
                    //t /= 1000000;
                    //uint32_t s = t % 60;
                    //t /= 60;
                    //uint32_t m = t % 60;
                    //t /= 60;
                    //uint32_t h = t % 24;

                    const char* logLevel =
                        logLevelNames[static_cast<size_t>(pSMI->severity_)];

                    //pos += sizeof(OneLogEntry);

                    CFMT_STR_OUTBUFARG(
                        fmtBuf, 
                        "%04d-%02d-%02d %02d:%02d:%02d,%06d %s %s %s->%s:%d] ",
                        now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
                        now->tm_hour, now->tm_min, now->tm_sec, us, 
                        logLevel, pTCP->name_, pSMI->filename_, 
                        pSMI->funcname_, pSMI->lineNum_);
                    pSMI->convertFN_(fmtBuf, pos + sizeof(OneLogEntry));
                    CFMT_STR_OUTBUFARG(fmtBuf, "\n");

                    //std::cout << outputFp_ << std::endl;
                    fwrite(fmtBuf.bufBegin(), 1, fmtBuf.getWrittenNum(), outputFp_);
                    //std::cout << fmtBuf.bufBegin() << std::endl;

                    pos += pOE->entrySize;
                    remaining -= static_cast<int>(pOE->entrySize);
                }

                bufLen -= (pTCP->blockSize_ + sizeof(ThreadCheckPoint));
            }
        }

        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        if (outputFp_) fflush(outputFp_);
    }

    if (outputFp_) fflush(outputFp_);
    delete[] cahce;
}

// RuntimeLogger constructor
RuntimeLogger::RuntimeLogger()
    : threadBuffers()
    , bufferMutex()
    , condMutex()
    , workAdded()
    , bgThread()
    , bgThreadShouldExit(false)
    , currentLogLevel(LogLevel::INFORMATION)
    , logCB(nullptr)
    , maxCBLogLevel(LogLevel::WARNING)
    , minCBPeriod(0)
    , sink_("SinkLogger.txt") {
    bgThread = std::thread(&RuntimeLogger::poll);
}

// RuntimeLogger destructor
RuntimeLogger::~RuntimeLogger() {
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

//void RuntimeLogger::setLogFile_internal(const char* filename) {
//    FILE* newFp = fopen(filename, "a");
//    if (!newFp) {
//        std::string err = "Unable to open file new log file: '";
//        err.append(filename);
//        err.append("': ");
//        err.append(strerror(errno));
//        throw std::ios_base::failure(err);
//    }
//
//    if (outputFp) fclose(outputFp);
//    outputFp = newFp;
//}

/**
 * Set where the NanoLog should output log. If a previous log file was 
 * specified, TZLog will attempt to sync() the remaining log
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
//void RuntimeLogger::setLogFile(const char* filename) {
//    tzLogSingleton.setLogFile_internal(filename);
//}

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

    size_t bytesWritten = 0;
    unsigned int count = 0;

    // Each iteration of this loop scans for uncompressed log messages in the
    // thread buffers, compresses as much as possible, and outputs it to a file.
    // The loop will run so long as it's not shutdown or there's outstanding I/O
    while (!bgThreadShouldExit) {
        bytesWritten = 0;

        {
            std::unique_lock<std::mutex> lock(bufferMutex);
            const int size = 32 / max(threadBuffers.size() / 8, 1);
            for (int i = 0; i < threadBuffers.size(); i++) {

                //std::cout << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";
                //std::cout << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";

                for (int j = 0; j < size; j++) {
                    std::cerr << "";
                }

                StagingBuffer* sb = threadBuffers[i];

                uint64_t peekBytes = 0;
                char* peekPosition = sb->peek(&peekBytes);

                // If there's work, unlock to perform it
                if (peekBytes > 0) {
                    count = 0;

                    lock.unlock();

                    //uint64_t remaining = peekBytes;
                    //while (remaining > 0) {
                    //    auto bytesToEncode =
                    //        (static_cast<uint64_t>(RELEASE_THRESHOLD)
                    //            < remaining) ? RELEASE_THRESHOLD : remaining;
                    //    sink_.append(peekPosition, bytesToEncode);
                    //    peekPosition += bytesToEncode;
                    //    remaining -= bytesToEncode;
                    //    sb->consume(bytesToEncode);
                    //}

                    //sink_.append(peekPosition, peekBytes);
                    //sb->consume(peekBytes);
                    sink_.appendNibble(peekPosition, peekBytes, sb);

                    bytesWritten += peekBytes;

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

        // If there's no data to output, go to sleep.
        if (bytesWritten == 0 && ++count >= NUMBER_OF_CHECKS_WITH_EMPTY_BUF) {
            std::unique_lock<std::mutex> lock(condMutex);
            workAdded.wait_for(
                lock, std::chrono::milliseconds(POLL_INTERVAL_NO_WORK_MS));
            //std::cout << "wait_for" << std::endl;
            count = 0;
        }
    }
}