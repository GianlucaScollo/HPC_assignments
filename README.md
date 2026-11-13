# HPC Assignments

Repository containing the code developed for the High Performance Computing assignments.

## Repository structure

```text
HPC_assignments/
├── OpenMP_assignment/
│   ├── linear-algebra/
│   │   └── kernels/
│   │       └── atax/
│   │           ├── README.md
│   │           ├── Makefile
│   │           ├── atax.c
│   │           ├── atax.h
│   │           ├── atax_seq.c
│   │           ├── atax_tmp.c
│   │           ├── atax_A.c
│   │           ├── atax_partial.c
│   │           ├── atax_validation.c
│   │           ├── atax_validation.h
│   │           └── References/
│   ├── scripts/
│   │   └── run_benchmarks.sh
│   ├── utilities/
│   └── README.md
├── .gitignore
└── README.md
```

The repository is organized by assignment. Each assignment has its own README with setup and execution instructions, while this file provides the general overview.

## Assignments

### OpenMP assignment

Contains the OpenMP implementations and experiments. The current kernel under development is ATAX, based on the PolyBench benchmark suite.

See [`OpenMP_assignment/README.md`](OpenMP_assignment/README.md) for setup instructions and [`OpenMP_assignment/linear-algebra/kernels/atax/README.md`](OpenMP_assignment/linear-algebra/kernels/atax/README.md) for ATAX-specific instructions.

## Authors

- **Name:** Gianluca Scollo - *UNIMORE ID:* 187575 - *Email:* 333517@studenti.unimore.it
- **Name:** Lorenzo Contri - *UNIMORE ID:* 222928 - *Email:* 394649@studenti.unimore.it
- **Name:** Tchinda Ngoune Samory - *UNIMORE ID:* 255111 - *Email:* 396047@studenti.unimore.it 

## Reproducing results

Each assignment should provide scripts and documented commands to reproduce its experiments. For the current OpenMP benchmark script, see [`OpenMP_assignment/scripts/run_benchmarks.sh`](OpenMP_assignment/scripts/run_benchmarks.sh).

Before running an experiment, check the README of the relevant assignment and make sure its prerequisites are installed.
