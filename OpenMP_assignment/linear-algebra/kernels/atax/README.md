# ATAX – OpenMP Implementations

This directory contains sequential and OpenMP implementations of the ATAX kernel from the PolyBench linear-algebra benchmarks.

ATAX computes:

```text
tmp = A * x
y   = A^T * tmp
```

where `A` is an `NX × NY` matrix and `x`, `tmp`, and `y` are vectors of the corresponding dimensions.

## Files

| File | Description |
|---|---|
| `atax.c` | Original reference implementation. |
| `atax_seq.c` | Sequential reference implementation. |
| `atax_tmp.c` | Parallelizes the computation of `tmp = A * x`; computes the final `y` sequentially. |
| `atax_A.c` | Parallelizes the first phase across matrix rows and the second phase across output-vector elements. |
| `atax_partial.c` | Uses thread-private partial output vectors and combines them to obtain the final `y`. |
| `atax_validation.c`, `atax_validation.h` | Save and compare output vectors against reference values. |
| `atax.h` | Shared kernel declarations and PolyBench definitions. |
| `References/` | Reference output files used by the validation code. |

The implementation names describe different parallelization strategies. In particular, `atax_partial.c` performs a manual reduction using per-thread partial vectors; it does not use the OpenMP `reduction` clause.

## Build configuration

The Makefile uses the shared build infrastructure in `utilities/`. It selects the implementation through `IMPL` and adds the validation source and header locally.

The ATAX Makefile should define these variables **before** including `c2.mk`:

```makefile
IMPL ?= atax_seq.c
EXTRA_SRC = atax_validation.c
EXTRA_HEADERS = atax_validation.h

-include ../../../utilities/options.mk
-include ../../../utilities/c2.mk
```

This allows the validation files to be included automatically, without specifying `EXTRA_SRC` and `EXTRA_HEADERS` on every command line.

## Dataset options

The PolyBench dataset is selected through a compiler definition:

| Dataset | Dimensions |
|---|---:|
| `MINI_DATASET` | 32 × 32 |
| `SMALL_DATASET` | 500 × 500 |
| `STANDARD_DATASET` | 4000 × 4000 |
| `LARGE_DATASET` | 8000 × 8000 |

The `EXTRALARGE_DATASET` is not included in the standard reproduction script because its memory requirements are very high.

## Build and run

Run the commands from this directory.

Sequential baseline, MINI dataset:

```bash
make IMPL=atax_seq.c EXT_CFLAGS="-DMINI_DATASET -DPOLYBENCH_TIME" clean all run
```

Other implementations can be selected by changing `IMPL`:

```bash
make IMPL=atax_A.c EXT_CFLAGS="-DSMALL_DATASET -DPOLYBENCH_TIME" clean all run

make IMPL=atax_partial.c EXT_CFLAGS="-DLARGE_DATASET -DPOLYBENCH_TIME" clean all run
```

If `IMPL ?= atax_seq.c` is set in the Makefile, the implementation can be omitted when running the sequential baseline.

By default, the OpenMP runtime determines the number of threads to use. You can optionally specify a fixed number of threads using `OMP_NUM_THREADS`:

```bash
OMP_NUM_THREADS=1 make IMPL=atax_partial.c EXT_CFLAGS="-DMINI_DATASET -DPOLYBENCH_TIME" clean all run

OMP_NUM_THREADS=2 make IMPL=atax_partial.c EXT_CFLAGS="-DMINI_DATASET -DPOLYBENCH_TIME" clean all run

OMP_NUM_THREADS=4 make IMPL=atax_partial.c EXT_CFLAGS="-DMINI_DATASET -DPOLYBENCH_TIME" clean all run
```

## Output validation

The validation helper compares the computed output vector with a reference vector saved from the sequential implementation. It first checks the dataset dimensions and then compares each element using combined absolute and relative tolerances:

```text
absolute_error <= atol + rtol * abs(reference)
```

The current tolerances are `atol = 1e-10` and `rtol = 1e-8`. This avoids requiring bit-for-bit equality when parallel summation changes the order of floating-point additions.

Make sure the reference files in `References/` were generated using the sequential implementation for the corresponding dataset. The validation function must be called by the program after the timed kernel execution if validation is to be included in a benchmark run.

## Reproducing performance runs

From `OpenMP_assignment/`, run:

```bash
chmod +x scripts/run_benchmarks.sh
./scripts/run_benchmarks.sh
```

The script runs the implementations and datasets listed there and stores the terminal output under `OpenMP_assignment/results/`. The output includes the PolyBench timing when `POLYBENCH_TIME` is enabled, along with any validation messages printed by the executable.

For a single implementation or dataset, use the commands above instead.

## Notes

- The sequential implementation is the reference for numerical validation.
- Small floating-point differences are expected in parallel implementations that change the order of accumulation.
- For fair performance comparisons, use the same dataset, compiler options, and number of threads.
- Generated executables and dependency files are not tracked by Git.
