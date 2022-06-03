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
#else
#include <sys/syscall.h>
#endif


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


#endif /* RUNTIME_LOGGER_H_ */