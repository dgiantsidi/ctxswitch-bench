#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <thread>
#include "timers.h"

constexpr int iterations = 20000000;
constexpr int items = 1000;


#if POLLUTE_CACHE
static inline void pollute_cache(std::unique_ptr<int[]>& tmp) {
    for (int i = 0; i < items; i++) {
        tmp[i] = 0;
        tmp[i] = tmp[items -i] + 1;
    }
#else
    static inline void pollute_cache() {
#endif

    }

#if POLLUTE_CACHE
    static void func(std::unique_ptr<int[]> tmp) {
#else
        static void func() {
#endif
            for (int i = 0; i < iterations; i++) {
                sched_yield();
#if POLLUTE_CACHE
                pollute_cache(tmp);
#endif
            }
        }

        int main(int args, char* argv[]) {
#if POLLUTE_CACHE
            std::unique_ptr<int[]> cur_tmp = std::make_unique<int[]>(items);
            std::unique_ptr<int[]> thd_tmp = std::make_unique<int[]>(items);
#endif
#if 0
            struct sched_param param;
            param.sched_priority = 1;
            if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
                fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
#endif

            struct timespec ts;
#if POLLUTE_CACHE
            std::thread thd(func, std::move(thd_tmp));
#else
            std::thread thd(func);
#endif

            auto start_ns = time_ns(&ts);
            auto start = rdtsc();

            for (int i = 0; i < iterations; i++) {
                sched_yield();
#if POLLUTE_CACHE
                pollute_cache(cur_tmp);
#endif
            }

            auto delta_cycles = rdtsc() - start;
            auto delta = time_ns(&ts) - start_ns;

            constexpr int nswitches = iterations << 2;
            fmt::print("{}\tctx_switches={}\ttime={}ns\t{}ns/ctxsw\tcycles={}\t\n", __func__,
                    nswitches, delta, (delta / (float) nswitches), delta_cycles);

            thd.join();
            return 0;
        }
