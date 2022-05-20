#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fmt/printf.h>

static inline uint64_t rdtsc(void) {
    unsigned long low, high;

    asm volatile("rdtsc" : "=a" (low), "=d" (high));

    return ((low) | (high) << 32);
}


static inline uint64_t time_ns(struct timespec* const ts) {
    if (clock_gettime(CLOCK_REALTIME, ts)) {
        exit(1);
    }
    return ((uint64_t) ts->tv_sec) * 1000000000LLU
        + (uint64_t) ts->tv_nsec;
}
