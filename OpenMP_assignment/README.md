# OpenMP Assignment

This directory contains the code and experiments for the OpenMP assignment.

## Contents

- `utilities/`: shared PolyBench support files and common Makefile infrastructure.
- `linear-algebra/kernels/atax/`: ATAX benchmark kernel and its implementations.
- `scripts/`: scripts used to reproduce benchmark runs.

## Requirements

The current ATAX implementation is built with:

- GCC with OpenMP support (`-fopenmp`);
- GNU Make;
- a Linux environment or another environment providing a compatible GCC/OpenMP toolchain.

The Makefile currently uses `gcc`, `-O2`, `-g`, and `-fopenmp`. PolyBench support code is included from `utilities/`.

## Build and run

The ATAX kernel has its own README containing the available implementations, dataset options, and complete commands:

[`linear-algebra/kernels/atax/README.md`](linear-algebra/kernels/atax/README.md)

For example, from the ATAX directory:

```bash
make IMPL=atax_seq.c EXT_CFLAGS="-DMINI_DATASET -DPOLYBENCH_TIME" clean all run
```

## Reproducing benchmark runs

From this directory, run:

```bash
chmod +x scripts/run_benchmarks.sh
./scripts/run_benchmarks.sh
```

The script runs the ATAX implementations listed in the script using the MINI, SMALL, STANDARD, and LARGE datasets. By default, the number of OpenMP threads is determined by the OpenMP runtime.

The script writes its output to `results/`.

You can optionally specify the number of threads through the `OMP_NUM_THREADS` environment variable:

```bash
OMP_NUM_THREADS=4 ./scripts/run_benchmarks.sh
```

**Important:** the script assumes that the implementation source files listed in it are present and that the ATAX Makefile includes the validation source and header automatically. Update the implementation list if filenames change.