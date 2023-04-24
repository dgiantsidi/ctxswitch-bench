
# ctxswitch-bench
Benchmark to measure the context switch and syscall costs in a native Linux environment and SGX (using SCONE which performs async and exit-less sycalls).

## Build and run
`make`

`sudo taskset -c 0 ./<bin-name>`

If you are running in NixOS, we provide the `default.nix` file. Execute `nix-shell` prior to building the binaries.


## Benchmarks
### ctxswitch
It spawns a thread that runs along with the main thread in the same cpu (`taskset`). They execute the `yield`-syscall to continously relinguish cpu. It measures the overhead of the scheduler. 

### ctxswitch-cache_pollution
It spawns a thread that runs along with the main thread in the same cpu (`taskset`). Each thread pollutes the cache by iterating a per-thread array and they execute the `yield`-syscall to continously relinguish cpu. It measures the overhead of the scheduler along with the cache-misses overhead.

### ctxswitch-pipes
It implements the ping-pong program among two threads.

### pipes
It measures the context of the pipe-syscall (single-thread that reads and writes to a pipe.).

### syscall
Measures a syscall cost by executing the `get_id()`-syscall. In `SCONE` with SGX support this did not worked.

### syscall_lat
Measures the latency of a syscall

### fibers_ctx
Instead of threads we use userlevel threading (fibers from `boost`). This was much faster.


### Command to run it on UoE scone version (not public)
`
Hugepagesize=2097152 \
        LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/7/ \
        SCONE_VERSION=1 SCONE_LOG=0 SCONE_QUEUES=8 SCONE_NO_FS_SHIELD=1 \
        SCONE_NO_MMAP_ACCESS=1 SCONE_HEAP=8G SCONE_LD_DEBUG=1 SCONE_MODE=hw \
        /opt/scone/lib/ld-scone-x86_64.so.1 ./executable
`

(from M)

`
Hugepagesize=2097152 LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/7/ SCONE_VERSION=1 SCONE_LOG=7 SCONE_NO_FS_SHIELD=1 SCONE_NO_MMAP_ACCESS=1 SCONE_HEAP=3584M SCONE_LD_DEBUG=1 /opt/scone/lib/ld-scone-x86_64.so.1 ./server ```
`

(From DG)

` 
sudo -E Hugepagesize=2097152 LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/7/ SCONE_VERSION=1 SCONE_LOG=0 SCONE_NO_FS_SHIELD=1 SCONE_NO_MMAP_ACCESS=1 SCONE_HEAP=16G SCONE_LD_DEBUG=1 SCONE_MODE=hw SCONE_ESPINS=100000000 SCONE_SSPINS=100000000 SCONE_CONFIG=scone_do.conf /opt/scone/lib/ld-scone-x86_64.so.1 ./db_bench --flagfile test-kernel
`
