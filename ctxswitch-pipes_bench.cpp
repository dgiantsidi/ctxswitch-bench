#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <thread>
#include <unistd.h>
#include "timers.h"

constexpr int iterations = 100000;
constexpr int items = 1000;

struct fd_pair {
    /*
       - pipefd[0] refers to the read end of the pipe.  
       - pipefd[1] refers to the write end of the pipe.
       */
    int fd[2];
};

struct fd_pair pipes_1;
struct fd_pair pipes_2;

static void func() {
    int buf = 0;
    for (int i = 0; i < iterations; i++) {
        [[maybe_unused]] auto ret = write(pipes_1.fd[1], &buf, 1);

        while (read(pipes_2.fd[0], &buf, 1) < 0) {
            fmt::print("{}", __func__);
        }

    }
    fmt::print("{} done\n", __func__);
}

int main(int args, char* argv[]) {
    if (pipe(pipes_1.fd) == -1) {
        fmt::print("{} pipe() error\n", __func__);
        exit(-1);
    }

    if (pipe(pipes_2.fd) == -1) {
        fmt::print("{} pipe() error\n", __func__);
        exit(-1);
    }

    struct sched_param param;
    param.sched_priority = 1;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
        fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

    struct timespec ts;
    std::thread thd(func);

    auto start_ns = time_ns(&ts);
    auto start = rdtsc();

    int buf = 0;
    for (int i = 0; i < iterations; i++) {
//   [[maybe_unused]] auto ret  = write(pipes_1.fd[1], &buf, 1);
        while (read(pipes_1.fd[0], &buf, 1) < 0) {};

        [[maybe_unused]] auto ret = write(pipes_2.fd[1], &buf, 1);
    }

    auto delta_cycles = rdtsc() - start;
    auto delta = time_ns(&ts) - start_ns;

    constexpr int nswitches = iterations << 2;
    fmt::print("{}\tctx_switches={}\ttime={}ns\t{}ns/ctxsw\tcycles={}\t\n", __func__,
            nswitches, delta, (delta / (float) nswitches), delta_cycles);

    thd.join();
    close(pipes_1.fd[0]);
    close(pipes_1.fd[1]);
    close(pipes_2.fd[0]);
    close(pipes_2.fd[1]);
    return 0;
}
