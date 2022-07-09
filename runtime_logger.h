/* Copyright (c) 2016-2020 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef RUNTIME_LOGGER_H_
#define RUNTIME_LOGGER_H_

#include <cassert>
#include <chrono>
#include <cstring>

#include <mutex>
#include <semaphore>
#include <atomic>
#include <string>
#include <thread>
#include <vector>
#include <condition_variable>

#include <stdint.h>

#ifdef _MSC_VER
#include <windows.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#else
#include <sys/syscall.h>
#endif

// Determines the byte size of the per-thread StagingBuffer that decouples
// the producer logging thread from the consumer background compression
// thread. This value should be large enough to handle bursts of activity.
#define STAGING_BUFFER_SIZE              (1 << 20)
#define BYTES_PER_CACHE_LINE             64
#define POLL_INTERVAL_NO_WORK_US         1000
#define NUMBER_OF_CHECKS_WITH_EMPTY_BUF  100

#if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#define ThreadLocal _Thread_local
#elif defined _WIN32 && ( \
       defined _MSC_VER || \
       defined __ICL || \
       defined __DMC__ || \
       defined __BORLANDC__ )
#define ThreadLocal __declspec(thread) 
/* note that ICC (linux) and Clang are covered by __GNUC__ */
#elif defined __GNUC__ || \
       defined __SUNPRO_C || \
       defined __xlC__
#define ThreadLocal __thread
#else
#error "Cannot define thread_local"
#endif

/**
 * This class is used to restrict instruction reordering within a CPU in
 * order to maintain synchronization properties between threads.  Is a thin
 * wrapper around x86 "fence" instructions.  Note: getting fencing correct
 * is extremely tricky!  Whenever possible, use existing solutions that already
 * handle the fencing.
 */
//class Fence {
//public:
//
//    /**
//     * This method creates a compiler level memory barrier forcing optimizer
//     * to not re-order memory accesses across the barrier. It can not prevent
//     * CPU reordering.
//     */
//    static void inline barrier() {
//        __asm__ __volatile__("" ::: "memory");
//    }
//
//    /**
//     * This method creates a boundary across which load instructions cannot
//     * migrate: if a memory read comes from code occurring before (after)
//     * invoking this method, the read is guaranteed to complete before (after)
//     * the method is invoked.
//     */
//    static void inline lfence() {
//        __asm__ __volatile__("lfence" ::: "memory");
//    }
//
//    /**
//     * This method creates a boundary across which store instructions cannot
//     * migrate: if a memory store comes from code occurring before (after)
//     * invoking this method, the store is guaranteed to complete before (after)
//     * the method is invoked.
//     */
//    static void inline sfence() {
//        __asm__ __volatile__("sfence" ::: "memory");
//    }
//
//    /**
//     * This method creates a serializing operation on all load-from-memory and
//     * store-to-memory instructions that were issued prior the mfence
//     * instruction. This serializing operation guarantees that every load and
//     * store instruction that precedes the mfence instruction in program order
//     * becomes globally visible before any load or store instruction that
//     * follows the mfence instruction. 
//     */
//    static void inline mfence() {
//        __asm__ __volatile__("mfence" ::: "memory");
//    }
//};


class TSCNS {
public:
    // If you haven't calibrated tsc_ghz on this machine, set tsc_ghz as 0.0
    // and it will auto wait 10 ms and calibrate. Of course you can calibrate
    // again later(e.g. after system init is done) and the longer you wait the
    // more precise tsc_ghz calibrate can get. It's a good idea that user waits
    // as long as possible(more than 1 min) once, and save the resultant
    // tsc_ghz returned from calibrate() somewhere(e.g. config file) on this
    // machine for future use. Or you can cheat, see README and cheat.cc for
    // details.
    //
    // If you have calibrated/cheated before on this machine as above, set
    // tsc_ghz and skip calibration.
    //
    // One more thing: you can re-init and calibrate TSCNS at later times if
    // you want to re-sync with system time in case of NTP or manual time
    // changes.
    double init(double tsc_ghz = 0.0) {
        syncTime(base_tsc, base_ns);
        if (tsc_ghz > 0) {
            tsc_ghz_inv = 1.0 / tsc_ghz;
            adjustOffset();
            return tsc_ghz;
        }
        else {
#ifdef _WIN32
            // wait more time as Windows' system time is in 100ns precision
            return calibrate(1000000 * 100);
#else
            return calibrate(1000000 * 10);
#endif
        }
    }

    double calibrate(uint64_t min_wait_ns = 10000000) {
        uint64_t delayed_tsc, delayed_ns;
        do {
            syncTime(delayed_tsc, delayed_ns);
        } while ((delayed_ns - base_ns) < min_wait_ns);

        tsc_ghz_inv = (double)(int64_t)(delayed_ns - base_ns)
            / (int64_t)(delayed_tsc - base_tsc);
        adjustOffset();
        return 1.0 / tsc_ghz_inv;
    }

    static uint64_t rdtsc() {
#ifdef _WIN32
        return __rdtsc();
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
        return __builtin_ia32_rdtsc();
#else
        return rdsysns();
#endif
    }

    uint64_t tsc2ns(uint64_t tsc) const { 
        return ns_offset + (int64_t)((int64_t)tsc * tsc_ghz_inv);
    }

    uint64_t rdns() const { return tsc2ns(rdtsc()); }

    // If you want cross-platform, use std::chrono as below which incurs one
    // more function call:
    static uint64_t rdsysns() {
        using namespace std::chrono;
        return static_cast<uint64_t>(duration_cast<nanoseconds>(
            system_clock::now().time_since_epoch()).count());
    }

    // For checking purposes, see test.cc
    uint64_t rdoffset() const { return ns_offset; }

private:
    // Linux kernel sync time by finding the first try with tsc diff < 50000
    // We do better: we find the try with the mininum tsc diff
    void syncTime(uint64_t& tsc, uint64_t& ns) {
        const int N = 10;
        uint64_t tscs[N + 1];
        uint64_t nses[N + 1];

        tscs[0] = rdtsc();
        for (int i = 1; i <= N; i++) {
            nses[i] = rdsysns();
            tscs[i] = rdtsc();
        }

        int best = 1;
        for (int i = 2; i <= N; i++) {
            if (tscs[i] - tscs[i - 1] < tscs[best] - tscs[best - 1]) best = i;
        }
        tsc = (tscs[best] + tscs[best - 1]) >> 1;
        ns = nses[best];
    }

    void adjustOffset() { 
        ns_offset = base_ns - (int64_t)((int64_t)base_tsc * tsc_ghz_inv); 
    }

    // make sure tsc_ghz_inv and ns_offset are on the same cache line
    alignas(64) double tsc_ghz_inv = 1.0;
    uint64_t ns_offset = 0;
    uint64_t base_tsc = 0;
    uint64_t base_ns = 0;
};

extern TSCNS tscns;
extern uint64_t midnight_ns;

/**
 * The levels of verbosity for messages logged with #TZ_LOG.
 */
enum class LogLevel {
    // Keep this in sync with logLevelNames
    SILENT_LOG_LEVEL = 0,

    // Bad stuff that shouldn't happen. The system broke its contract to
    // users in some way or some major assumption was violated.
    ERR,

    // Messages at the WARNING level indicate that, although something went
    // wrong or something unexpected happened, it was transient and
    // recoverable.
    WARNING,

    // Somewhere in between WARNING and DEBUG...
    INFORMATION,

    // Messages at the DEBUG level don't necessarily indicate that anything
    // went wrong, but they could be useful in diagnosing problems.
    /*DEBUG*/DEB,

    // must be the last element in the enum
    NUM_LOG_LEVELS
};

typedef void (*LogCBFn)(uint64_t ns, LogLevel level, const char* msg, size_t msg_len);

/**
 * RuntimeLogger provides runtime support to the C++ code generated by the
 * Preprocessor component.
 * Its main responsibilities are to manage fast thread-local storage to stage
 * uncompressed log messages and manage a background thread to compress the
 * log messages to an output file.
 */
class RuntimeLogger {
public:
    /**
     * Allocate thread-local space for the generated C++ code to store an
     * log message, but do not make it available for consumer thread
     * yet. The caller should invoke finishAlloc() to make the space visible
     * to the compression thread and this function shall not be invoked
     * again until the corresponding finishAlloc() is invoked first.
     *
     * Note this will block of the buffer is full.
     *
     * \param nbytes
     *      number of bytes to allocate in the
     *
     * \return
     *      pointer to the allocated space
     */
    static inline char* reserveAlloc(size_t nbytes) {
        if (stagingBuffer == nullptr) 
            tzLogSingleton.ensureStagingBufferAllocated();

        // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
        return stagingBuffer->reserveProducerSpace(nbytes);
    }

    /**
     * Complement to reserveAlloc, makes the bytes previously
     * reserveAlloc()-ed visible to the compression/output thread.
     *
     * \param nbytes
     *      Number of bytes to make visible
     */
    static inline void finishAlloc(size_t nbytes) { 
        stagingBuffer->finishReservation(nbytes); 
    }

    static void preallocate();
    static void setLogFile(const char* filename);
    static void setLogLevel(LogLevel logLevel);
    static void poll() { tzLogSingleton.poll_(); }

    static inline LogLevel getLogLevel() { return tzLogSingleton.currentLogLevel; }

    static void setThreadName(const char* name) {
        tzLogSingleton.ensureStagingBufferAllocated();
        stagingBuffer->setName(name);
    }

    static void setLogCB(LogCBFn cb, LogLevel maxCBLogLevel, uint64_t minCBPeriodInSec) {
        tzLogSingleton.setLogCB_(cb, maxCBLogLevel, minCBPeriodInSec);
    }

    static const RuntimeLogger& getTZLog() {
        return tzLogSingleton;
    }

    inline void ensureStagingBufferAllocated() {
        if (stagingBuffer == nullptr) {
            std::unique_lock<std::mutex> guard(bufferMutex);
            // Unlocked for the expensive StagingBuffer allocation
            guard.unlock();
            stagingBuffer = new StagingBuffer();
            guard.lock();

            threadBuffers.push_back(stagingBuffer);
        }
    }

private:
    // Forward Declarations
    class StagingBuffer;
    class StagingBufferDestroyer;

    // Storage for staging uncompressed log statements for compression
    static ThreadLocal StagingBuffer* stagingBuffer;

    // Destroys the __thread StagingBuffer upon its own destruction, which
    // is synchronized with thread death
    static thread_local StagingBufferDestroyer sbc;

    // Singleton RuntimeLogger that manages the thread-local structures and
    // background output thread.
    static RuntimeLogger tzLogSingleton;

    RuntimeLogger();

    ~RuntimeLogger();

    void setLogFile_internal(const char* filename);

    void poll_();

    void setLogCB_(LogCBFn cb, LogLevel maxCBLogLevel_, uint64_t minCBPeriodInSec) {
        logCB = cb;
        maxCBLogLevel = maxCBLogLevel_;
        minCBPeriod = minCBPeriodInSec * 1000000000;
    }

    /**
     * Allocates thread-local structures if they weren't already allocated.
     * This is used by the generated C++ code to ensure it has space to
     * log messages to and by the user if they wish to preallocate the data
     * structures on thread creation.
     */
    //inline void ensureStagingBufferAllocated() {
    //    if (stagingBuffer == nullptr) {
    //        std::unique_lock<std::mutex> guard(bufferMutex);
    //        // Unlocked for the expensive StagingBuffer allocation
    //        guard.unlock();
    //        stagingBuffer = new StagingBuffer();
    //        guard.lock();

    //        threadBuffers.push_back(stagingBuffer);
    //    }
    //}

    // Globally the thread-local stagingBuffers
    std::vector<StagingBuffer*> threadBuffers;

    // Protects reads and writes to threadBuffers
    std::mutex bufferMutex;

    // Protects the condition variables below
    std::mutex condMutex;
    std::binary_semaphore condSmph;

    // Signal for when the poll thread should wakeup
    std::condition_variable workAdded;

    // Background thread that polls the various staging buffers and outputs it to
    // a sink buffer.
    std::thread bgThread;

    // Flag signaling the bgThread to stop running. This is typically only set in
    // testing or when the application is exiting.
    bool bgThreadShouldExit;

    FILE* outputFp;

    // Minimum log level that RuntimeLogger will accept. Anything lower will
    // be dropped.
    LogLevel currentLogLevel;

    LogCBFn logCB;

    LogLevel maxCBLogLevel;

    uint64_t minCBPeriod;


    /**
     * Implements a circular FIFO producer/consumer byte queue that is used
     * to hold the dynamic information of a NanoLog log statement (producer)
     * as it waits for compression via the NanoLog background thread
     * (consumer). There exists a StagingBuffer for every thread that uses
     * the NanoLog system.
     */
    class StagingBuffer {
    public:
        /**
         * Attempt to reserve contiguous space for the producer without
         * making it visible to the consumer. The caller should invoke
         * finishReservation() before invoking reserveProducerSpace()
         * again to make the bytes reserved visible to the consumer.
         *
         * This mechanism is in place to allow the producer to initialize
         * the contents of the reservation before exposing it to the
         * consumer. This function will block behind the consumer if
         * there's not enough space.
         *
         * \param nbytes
         *      Number of bytes to allocate
         * \param blocking
         *      Test parameter that indicates that the function should
         *      return with a nullptr rather than block when there's
         *      not enough space. default is true
         *
         * \return
         *      Pointer to at least nbytes of contiguous space
         */
        inline char* reserveProducerSpace(size_t nbytes, bool blocking = true) {
            auto currentProducerPos =
                _producerPos.load(std::memory_order_relaxed);

            // Fast in-line path
            if (nbytes < _minFreeSpace)
                return _storage + currentProducerPos;

            // Slow allocation
            // There's a subtle point here, all the checks for remaining
            // space are strictly < or >, not <= or => because if we allow
            // the record and print positions to overlap, we can't tell
            // if the buffer either completely full or completely empty.
            // Doing this check here ensures that == means completely empty.
            tzLogSingleton.workAdded.notify_all();
            while (_minFreeSpace <= nbytes) {
                // Since _consumerPos can be updated in a different thread, we save
                // a consistent copy of it here to do calculations on
                const auto cachedConsumerPos =
                    _consumerPos.load(std::memory_order_acquire);

                if (cachedConsumerPos <= currentProducerPos) {
                    _minFreeSpace = STAGING_BUFFER_SIZE - currentProducerPos;

                    if (_minFreeSpace > nbytes)
                        break;

                    // Not enough space at the end of the buffer; wrap around
                    _endOfRecordedSpace.store(currentProducerPos,
                        std::memory_order_relaxed);

                    // Prevent the roll over if it overlaps the two positions because
                    // that would imply the buffer is completely empty when it's not.
                    // Doing this ensures that _producerPos is always less than 
                    // cachedConsumerPos when rolled over to storage.
                    if (cachedConsumerPos != 0) { // 0 < cachedConsumerPos
                        // prevents _producerPos from updating before 
                        // _endOfRecordedSpace
                        _producerPos.store(0, std::memory_order_release);
                        currentProducerPos =
                            _producerPos.load(std::memory_order_relaxed);
                        _minFreeSpace = cachedConsumerPos - currentProducerPos;
                    }
                }
                else { // cachedConsumerPos > currentProducerPos
                    _minFreeSpace = cachedConsumerPos - currentProducerPos;
                }

                // Needed to prevent infinite loops in tests
                if (!blocking && _minFreeSpace <= nbytes)
                    return nullptr;
            }

            // minFreeSpace > nbytes
            return _storage + currentProducerPos;
        }

        /**
         * Complement to reserveProducerSpace that makes nbytes starting
         * from the return of reserveProducerSpace visible to the consumer.
         *
         * \param nbytes
         *      Number of bytes to expose to the consumer
         */
        inline void finishReservation(size_t nbytes) {
            assert(nbytes < _minFreeSpace);
            const auto currentProducerPos =
                _producerPos.load(std::memory_order_relaxed);

            //std::cout << currentProducerPos /*+ nbytes*/ << std::endl;
            assert(currentProducerPos + nbytes < STAGING_BUFFER_SIZE);
            _producerPos.store(currentProducerPos + nbytes,
                std::memory_order_release);
            _minFreeSpace -= nbytes;
        }

        /**
        * Peek at the data available for consumption within the stagingBuffer.
        * The consumer should also invoke consume() to release space back
        * to the producer. This can and should be done piece-wise where a
        * large peek can be consume()-ed in smaller pieces to prevent blocking
        * the producer.
        *
        * \param[out] bytesAvailable
        *      Number of bytes consumable
        * \return
        *      Pointer to the consumable space
        */
        char* peek(uint64_t* bytesAvailable) {
            auto currentConsumerPos =
                _consumerPos.load(std::memory_order_relaxed);

            // Save a consistent copy of _producerPos
            const auto cachedProducerPos =
                _producerPos.load(std::memory_order_acquire);

            if (cachedProducerPos < currentConsumerPos) {
                // Prevent reading new producerPos but old endOfRecordedSpace
                *bytesAvailable =
                    _endOfRecordedSpace.load(std::memory_order_relaxed)
                    - currentConsumerPos;

                if (*bytesAvailable > 0)
                    return _storage + currentConsumerPos;

                // Roll over (_endOfRecordedSpace == currentConsumerPos)
                _consumerPos.store(0, std::memory_order_release);
                currentConsumerPos = _consumerPos.load(std::memory_order_relaxed);
            }

            // cachedProducerPos >= currentConsumerPos
            // here ensures that == means consumable space is completely empty.
            *bytesAvailable = cachedProducerPos - currentConsumerPos;
            return _storage + currentConsumerPos;
        }

        /**
        * Consumes the next nbytes in the StagingBuffer and frees it back
        * for the producer to reuse. nbytes must be less than what is
        * returned by peek().
        *
        * \param nbytes
        *      Number of bytes to return back to the producer
        */
        inline void consume(uint64_t nbytes) {
            // Make sure consumer reads finish before bump
            const auto currentConsumerPos =
                _consumerPos.load(std::memory_order_relaxed);
            _consumerPos.store(currentConsumerPos + nbytes,
                std::memory_order_release);
        }

        void setName(const char* name) { strncpy(_name, name, sizeof(_name) - 1); }

        const char* getName() { return _name; }

        /**
         * Returns true if it's safe for the compression thread to delete
         * the StagingBuffer and remove it from the global vector.
         *
         * \return
         *      true if its safe to delete the StagingBuffer
         */
        bool checkCanDelete() { return _shouldDeallocate.load(); }

        bool isLockFree() const {
            return (_producerPos.is_lock_free()
                && _endOfRecordedSpace.is_lock_free()
                && _consumerPos.is_lock_free());
        }

        StagingBuffer()
            : _producerPos(0/*_storage*/)
            , _endOfRecordedSpace(/*_storage + */STAGING_BUFFER_SIZE)
            , _minFreeSpace(STAGING_BUFFER_SIZE)
            , _cacheLineSpacer()
            , _consumerPos(0/*_storage*/)
            , _shouldDeallocate(false)
            , _storage() {
            // Empty function, but causes the C++ runtime to instantiate the
            // sbc thread_local (see documentation in function).
            sbc.stagingBufferCreated();

#ifdef _WIN32
            unsigned long tid = static_cast<unsigned long>(GetCurrentThreadId());
            snprintf(_name, sizeof(_name), "%lu", tid);
#else
            uint32_t tid = static_cast<pid_t>(::syscall(SYS_gettid));
            snprintf(_name, sizeof(_name), "%u", tid);
#endif
        }

        ~StagingBuffer() {}

    private:
        char _name[16] = { 0 };

        // Position within storage[] where the producer may place new data
        // char* producerPos;
        std::atomic <size_t> _producerPos;
        //char* _producerPos;

        // Marks the end of valid data for the consumer. Set by the producer
        // on a roll-over
        // char* endOfRecordedSpace;
        std::atomic <size_t> _endOfRecordedSpace;
        //char* _endOfRecordedSpace;

        // Lower bound on the number of bytes the producer can allocate w/o
        // rolling over the producerPos or stalling behind the consumer
        uint64_t _minFreeSpace;

        // An extra cache-line to separate the variables that are primarily
        // updated/read by the producer (above) from the ones by the
        // consumer(below)
        char _cacheLineSpacer[BYTES_PER_CACHE_LINE];

        // Position within the storage buffer where the consumer will consume
        // the next bytes from. This value is only updated by the consumer.
        // char* volatile consumerPos;
        std::atomic <size_t> _consumerPos;
        //char* /*volatile*/ _consumerPos;

        // Indicates that the thread owning this StagingBuffer has been
        // destructed (i.e. no more messages will be logged to it) and thus
        // should be cleaned up once the buffer has been emptied by the
        // compression thread.
        std::atomic<bool> _shouldDeallocate;

        // Backing store used to implement the circular queue
        char _storage[STAGING_BUFFER_SIZE];

        friend RuntimeLogger;
        friend StagingBufferDestroyer;

        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;
    };


    // This class is intended to be instantiated as a C++ thread_local to
    // synchronize marking the thread local stagingBuffer for deletion with
    // thread death.
    //
    // The reason why this class exists rather than wrapping the stagingBuffer
    // in a unique_ptr or declaring the stagingBuffer itself to be thread_local
    // is because of performance. Dereferencing the former costs 10 ns and the
    // latter allocates large amounts of resources for every thread that is
    // created, which is wasteful for threads that do not use the RuntimeLogger.
    class StagingBufferDestroyer {
    public:
        // TODO(syang0) I wonder if it'll be better if stagingBuffer was
        // actually a thread_local wrapper with dereference operators
        // implemented.

        explicit StagingBufferDestroyer() {}

        // Weird C++ hack; C++ thread_local are instantiated upon first use
        // thus the StagingBuffer has to invoke this function in order
        // to instantiate this object.
        void stagingBufferCreated() {}

        virtual ~StagingBufferDestroyer() {
            if (stagingBuffer != nullptr) {
                stagingBuffer->_shouldDeallocate.store(true);
                stagingBuffer = nullptr;
            }
        }
    };

}; // RuntimeLogger

#endif /* RUNTIME_LOGGER_H_ */



