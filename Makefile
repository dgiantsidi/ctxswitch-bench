
CXX = g++
CXX_FLAGS = -Wall -O3 -g -std=c++17
LD_FLAGS = -lfmt -pthread
BOOST_FLAGS = -lboost_fiber -lboost_context


all:
	$(CXX)  $(CXX_FLAGS) ctxswitch-pipes_bench.cpp -o ctxswitch-pipes $(LD_FLAGS)
	$(CXX)  $(CXX_FLAGS) pipes_bench.cpp -o pipes $(LD_FLAGS)
	$(CXX) -DPOLLUTE_CACHE=1 $(CXX_FLAGS) ctxswitch_bench.cpp -o ctxswitch-cache_pollution $(LD_FLAGS)
	$(CXX) -DPOLLUTE_CACHE=0 $(CXX_FLAGS) ctxswitch_bench.cpp -o ctxswitch $(LD_FLAGS)
	$(CXX) -DPOLLUTE_CACHE=0 $(CXX_FLAGS) syscall_bench.cpp -o syscall $(LD_FLAGS)
	$(CXX) -DLATENCY=1 $(CXX_FLAGS) syscall_bench.cpp -o syscall_lat $(LD_FLAGS)
	$(CXX) -DLATENCY=1 $(CXX_FLAGS) fibers-ctx_bench.cpp -o fibers_ctx $(LD_FLAGS) $(BOOST_FLAGS)
