#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fmt/printf.h>
#include <iostream>
#include <memory>
#include <thread>
#include "timers.h"

#if LATENCY
constexpr uint64_t iterations = 100000;
#else
constexpr uint64_t iterations = 5000000;
#endif

int main(int args, char* argv[]) {
#if RAII
#warning "RAII is defined"
    std::unique_ptr<uint32_t[]> results = std::make_unique<uint32_t[]>(iterations);
    memset(results.get(), 0, iterations);

    // lock the pages of the virtual address space into RAM to prevent from swapping
    while(0 != mlock(results.get(), sizeof(uint32_t)*iterations)) {}
#else
    uint32_t* results = new uint32_t[iterations];
    memset(results, 0, iterations);

    // lock the pages of the virtual address space into RAM to prevent from swapping
    while(0 != mlock(results, sizeof(uint32_t)*iterations)) {}
#endif
    

    struct timespec ts;
    auto start_ns = time_ns(&ts);

    auto start = rdtsc();
    auto start2 = start;
    auto tmp = rdtsc();
    for (uint64_t i = 0; i < iterations; i++) {
        // execute syscall(SYS_gettid) but this is not implemented in SCONE
        std::this_thread::get_id();
#if LATENCY
        tmp = rdtsc();
        results[i]= tmp - start;
        start = tmp;
#endif
    }

    auto stop = rdtsc();
    const uint64_t delta = time_ns(&ts) - start_ns;
    uint64_t sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += results[i];
    }

    fmt::print("{}\titerations={}\tcycles={}\tavg_lat={}cycles/syscall\n", __func__, iterations, sum, (float) sum/ ((float) iterations));
    fmt::print("{}\titerations={}\tavg_cycles={}\tavg_lat={}cycles/syscall\n", __func__, iterations, (stop-start2), (float) (stop-start2)/ ((float) iterations));
    fmt::print("{}\titerations={}\ttime={}ns\tavg_lat={}ns/syscall\n", __func__, iterations, delta, delta / ((float) iterations));
#ifndef RAII
    delete[] results;
#endif
    
    return 0;
}
