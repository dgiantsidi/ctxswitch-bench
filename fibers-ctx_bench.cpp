#include <thread>
#include <iostream>
#include <time.h>
#include <vector>

#include <boost/fiber/all.hpp>
#include "timers.h"

constexpr uint64_t iterations = 100000000;

void func1() {
  for (int i = 0; i < iterations*2; i++) {
    boost::this_fiber::yield();
  }
}

void func2() {
  struct timespec ts;
  auto start_ns = time_ns(&ts);
  auto start = rdtsc();

  for (int i = 0; i < iterations; i++) {
    boost::this_fiber::yield();
  }
  auto delta_cycles = rdtsc() - start;
  auto delta = time_ns(&ts) - start_ns;

  constexpr int nswitches = iterations << 2;
  fmt::print("{}\tctx_switches={}\ttime={}ns\t{}ns/ctxsw\tcycles={}\tavg_lat={}cycles/ctx\n", __func__,
      nswitches, delta, (delta / (float) nswitches), delta_cycles, delta_cycles/nswitches);

}


int main(int argc, char* argv[]) {
  std::vector<boost::fibers::fiber> fibers;

  int nb_fibers = 2;
  fibers.emplace_back(boost::fibers::launch::post, func1);
  fibers.emplace_back(boost::fibers::launch::post, func2);

  std::cout << "fibers launched .. good luck!\n";

  for (auto& fiber : fibers)
    fiber.join();

  std::cout << "all fibers joined\n";
}
