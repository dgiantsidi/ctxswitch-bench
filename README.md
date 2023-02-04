
# ctxswitch-bench
Benchmark to measure the context switch cost.

## Build and run
`make` \\
`sudo taskset -c 0 ./<bin-name>`

If you are running in NixOS, we provide the `default.nix` file. Execute `nix-shell` prior to building the binaries.
