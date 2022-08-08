#include <stdint.h>
#include <iostream>
#include <iterator>
#include <ctime>
#include <locale>

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
    , newBufferCount_(0)
    , running_(false)
    , basename_(basename)
    //, rollSize_(rollSize)
    , thread_()
    //, latch_(1)
    , mutex_()
    , cond_()
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
    , buffers_() 
    //, newBuffers_() 
    , newBufferCachePool_() {

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
    //newBuffers_.reserve(SUPPLEMENTARY_SINKBUFFER_MAXIMUM_SIZE);
    thread_ = std::thread(std::bind(&SinkLogger::threadFunc, this));
}

void RuntimeLogger::SinkLogger::appendInternal(const char* logline, int len,
                                               StagingBuffer* sb) {
    ThreadCheckPoint* pTCP =
        new(currentBuffer_->current()) ThreadCheckPoint();
    //ThreadCheckPoint* pTCP =
    //    reinterpret_cast<ThreadCheckPoint*>(currentBuffer_->current());
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
}

void RuntimeLogger::SinkLogger::append(const char* logline, int len,
                                       StagingBuffer* sb, 
                                       bool blocking/* = true*/) {
    do {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentBuffer_->avail() > (len + sizeof(ThreadCheckPoint))) {
            appendInternal(logline, len, sb);
        }
        else {
            if (nextBuffer_) {
                buffers_.push_back(std::move(currentBuffer_));
                currentBuffer_ = std::move(nextBuffer_);
            }
            // Rarely happens
            //else if (!newBufferCachePool_.empty()) {
            //    //std::cout << newBufferCachePool_.size() << std::endl;
            //    buffers_.push_back(std::move(currentBuffer_));
            //    currentBuffer_ = std::move(newBufferCachePool_.back());
            //    newBufferCachePool_.pop_back();
            //}
            else {
                if (newBufferCount_.load(std::memory_order_relaxed) 
                    < SUPPLEMENTARY_SINKBUFFER_MAXIMUM_SIZE) {
                    buffers_.push_back(std::move(currentBuffer_));
                    currentBuffer_.reset(new Buffer);
                    ++newBufferCount_;
                    //std::cout << newBufferCount_ << std::endl;
                }
                else {
                    //std::cout << newBufferCount_ << std::endl;
                    if (!blocking) sb->consume(len);
                    continue;
                }
            }
            appendInternal(logline, len, sb);
            cond_.notify_all();
        }
        blocking = false;
    } while (blocking &&
             [](std::condition_variable& cond) {
                 cond.notify_all();
                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
                 return true; }(cond_));
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
    BufferList buffersToCache;
    buffersToWrite.reserve(16);
    char* cahce = new char[FORMAT_BUFFER_SIZE];
    OutbufArg fmtBuf(cahce, FORMAT_BUFFER_SIZE);
    static const char* logLevelNames[] = { "NAN", "ERR", "WRN", "INF", "DBG" };

    std::time_t lastTimer = static_cast<time_t>(0);
    char dateCache[100];
    std::tm* last = nullptr;

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
                if (currentBuffer_->length() > 
                    0.8 * currentBuffer_->bufferSize()) {
                    buffers_.push_back(std::move(currentBuffer_));
                    currentBuffer_ = std::move(newBuffer1);
                }
            }
            else { // std::cv_status::timeout == status
                //if (!newBufferCachePool_.empty()) {
                //    //newBufferCount_ -= newBufferCachePool_.size();
                //    newBufferCount_.fetch_sub(
                //        newBufferCachePool_.size(), std::memory_order_relaxed);
                //    newBufferCachePool_.clear();
                //}
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
            //if (!buffersToCache.empty()) {
            //    newBufferCachePool_.splice(
            //        newBufferCachePool_.end(), buffersToCache);
            //}
        }

        //std::cout << buffersToWrite.size() << std::endl;

        if (buffersToWrite.empty()) continue;

        assert(!buffersToWrite.empty());

        //if (buffersToWrite.size() > 25) {
        //    char buf[256];
        //    snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
        //        Timestamp::now().toFormattedString().c_str(),
        //        buffersToWrite.size() - 2);
        //    fputs(buf, stderr);
        //    output.append(buf, static_cast<int>(strlen(buf)));
        //    buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        //}

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

                    //std::tm* now = std::localtime(&timer);
                    std::tm* now = last;
                    if (timer != lastTimer) {
                        now = std::localtime(&timer);
                        last = now;
                        lastTimer = timer;
                        /*std::size_t count = std::strftime(
                            dateCache, sizeof(dateCache), "%Y-%m-%d %H:%M:%S",
                            std::localtime(&timer));
                        if (0 == count) {
                            std::strcpy(dateCache, "illegal date string length");
                        }*/
                    }

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
                        now->tm_hour, now->tm_min, now->tm_sec,
                        /*"%s,%06d %s %s %s->%s:%d] ",
                        dateCache,*/
                        us, logLevel, pTCP->name_, pSMI->filename_,
                        pSMI->funcname_, pSMI->lineNum_);

                    if (internal::LogEntryStatus::NORMAL
                        == pOE->status) [[likely]] {
                        pSMI->convertFN_(fmtBuf, pos + sizeof(OneLogEntry));
                        CFMT_STR_OUTBUFARG(fmtBuf, "\n");
                    }
                    else if (internal::LogEntryStatus::ILLEGAL_ARGS_SIZE
                        == pOE->status) [[unlikely]] {
                        auto argSize = *reinterpret_cast<const size_t*>(
                            pos + sizeof(OneLogEntry));
                        CFMT_STR_OUTBUFARG(
                            fmtBuf,
                            "<ILLEGAL_ARGS_SIZE>"
                            "\n\tThe total size of the arguments to be formatted by TZLog is [%u]"
                            ", exceeding the maximum value allowed [%d], "
                            "please check the log input!!!\n",
                            static_cast<uint32_t>(argSize), FORMAT_ARGS_MAXIMUM_SIZE);
                    }

                    size_t size = 
                        fmtBuf.getWrittenNum() < (FORMAT_BUFFER_SIZE - 1) 
                        ? fmtBuf.getWrittenNum() : (FORMAT_BUFFER_SIZE - 1);
                    //fwrite(fmtBuf.bufBegin(), 1, size, outputFp_);

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

        //newBufferCount_ = 0;
        newBufferCount_.store(0, std::memory_order_relaxed);

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

        //if (!buffersToWrite.empty()) {
        //    assert(buffersToCache.size() == 0);
        //    for (auto& buffer : buffersToWrite) {
        //        buffer->reset();
        //        buffersToCache.push_back(std::move(buffer));
        //    }
        //    //std::move(std::begin(buffersToWrite), std::end(buffersToWrite), 
        //    //          std::back_inserter(newBuffers_));
        //}

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
            //const int size = 32 / max(threadBuffers.size() / 8, 1);
            const auto size1 = threadBuffers.size() / 8;
            const auto size2 = 32 / (size1 > 1 ? size1 : 1);
            for (int i = 0; i < threadBuffers.size(); i++) {

                //std::cout << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";
                //std::cout << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";

                for (int j = 0; j < size2; j++) {
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
                    sink_.append(peekPosition, peekBytes, sb);

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