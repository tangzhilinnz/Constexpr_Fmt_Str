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

#include <mutex>
#include <string>
#include <thread>
#include <vector>

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
#define STAGING_BUFFER_SIZE    1 << 20
#define BYTES_PER_CACHE_LINE   64


/**
 * This class is used to restrict instruction reordering within a CPU in
 * order to maintain synchronization properties between threads.  Is a thin
 * wrapper around x86 "fence" instructions.  Note: getting fencing correct
 * is extremely tricky!  Whenever possible, use existing solutions that already
 * handle the fencing.
 */
class Fence {
public:

    /**
     * This method creates a compiler level memory barrier forcing optimizer
     * to not re-order memory accesses across the barrier. It can not prevent
     * CPU reordering.
     */
    static void inline barrier() {
        __asm__ __volatile__("" ::: "memory");
    }

    /**
     * This method creates a boundary across which load instructions cannot
     * migrate: if a memory read comes from code occurring before (after)
     * invoking this method, the read is guaranteed to complete before (after)
     * the method is invoked.
     */
    static void inline lfence() {
        __asm__ __volatile__("lfence" ::: "memory");
    }

    /**
     * This method creates a boundary across which store instructions cannot
     * migrate: if a memory store comes from code occurring before (after)
     * invoking this method, the store is guaranteed to complete before (after)
     * the method is invoked.
     */
    static void inline sfence() {
        __asm__ __volatile__("sfence" ::: "memory");
    }

    /**
     * This method creates a serializing operation on all load-from-memory and
     * store-to-memory instructions that were issued prior the mfence
     * instruction. This serializing operation guarantees that every load and
     * store instruction that precedes the mfence instruction in program order
     * becomes globally visible before any load or store instruction that
     * follows the mfence instruction. 
     */
    static void inline mfence() {
        __asm__ __volatile__("mfence" ::: "memory");
    }
};


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
     *
     * \return
     *      Pointer to at least nbytes of contiguous space
     */
    inline char* reserveProducerSpace(size_t nbytes) {
        ++numAllocations;

        // Fast in-line path
        if (nbytes < minFreeSpace)
            return producerPos;

        // Slow allocation
        return reserveSpaceInternal(nbytes);
    }

    /**
     * Complement to reserveProducerSpace that makes nbytes starting
     * from the return of reserveProducerSpace visible to the consumer.
     *
     * \param nbytes
     *      Number of bytes to expose to the consumer
     */
    inline void finishReservation() { 
        assert(nbytes < minFreeSpace);
        assert(producerPos + nbytes < storage + STAGING_BUFFER_SIZE);

        Fence::sfence(); // Ensures producer finishes writes before bump
        minFreeSpace -= nbytes;
        producerPos += nbytes;
        // producerPos += nbytes is divided into three steps, 
        // each of which is atomic operation: 
        // 1) read producerPos
        // 2) add nbytes to producerPos
        // 3) store the new value to producerPos
        // the brand new producerPos is visible to consumer thread 
        // only when the third step is done, otherwise the stale value
        // is used. 
    }

    MsgHeader* peek() { return varq.front(); }

    inline void consume() { varq.pop(); }

    void setName(const char* name_) { strncpy(name, name_, sizeof(name) - 1); }

    const char* getName() { return name; }

    /**
     * Returns true if it's safe for the compression thread to delete
     * the StagingBuffer and remove it from the global vector.
     *
     * \return
     *      true if its safe to delete the StagingBuffer
     */
    bool checkCanDelete() { return shouldDeallocate; }

    StagingBuffer()
        : producerPos(storage)
        , endOfRecordedSpace(storage + STAGING_BUFFER_SIZE)
        , minFreeSpace(STAGING_BUFFER_SIZE)
        , cacheLineSpacer()
        , consumerPos(storage)
        , shouldDeallocate(false)
        , storage()
        , shouldDeallocate(false) {
        // Empty function, but causes the C++ runtime to instantiate the
        // sbc thread_local (see documentation in function).
        sbc.stagingBufferCreated();

#ifdef _WIN32
        unsigned long tid = static_cast<unsigned long>(GetCurrentThreadId());
        snprintf(name, sizeof(name), "%lu", tid);
#else
        uint32_t tid = static_cast<pid_t>(::syscall(SYS_gettid));
        snprintf(name, sizeof(name), "%u", tid);
#endif
    }

    ~StagingBuffer() {}

private:
    char name[16] = { 0 };

    char* reserveSpaceInternal(size_t nbytes, bool blocking = true);

    // Position within storage[] where the producer may place new data
    char* producerPos;

    // Marks the end of valid data for the consumer. Set by the producer
    // on a roll-over
    char* endOfRecordedSpace;

    // Lower bound on the number of bytes the producer can allocate w/o
    // rolling over the producerPos or stalling behind the consumer
    uint64_t minFreeSpace;

    // An extra cache-line to separate the variables that are primarily
    // updated/read by the producer (above) from the ones by the
    // consumer(below)
    char cacheLineSpacer[BYTES_PER_CACHE_LINE];

    // Position within the storage buffer where the consumer will consume
    // the next bytes from. This value is only updated by the consumer.
    char* volatile consumerPos;

    // Indicates that the thread owning this StagingBuffer has been
    // destructed (i.e. no more messages will be logged to it) and thus
    // should be cleaned up once the buffer has been emptied by the
    // compression thread.
    bool shouldDeallocate;

    // Backing store used to implement the circular queue
    char storage[STAGING_BUFFER_SIZE];

    friend RuntimeLogger;
    friend StagingBufferDestroyer;

    StagingBuffer(const StagingBuffer&) = delete;
    StagingBuffer& operator=(const StagingBuffer&) = delete;
};

#endif /* RUNTIME_LOGGER_H_ */