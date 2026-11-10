
#!/usr/bin/env bash
set -euo pipefail

# Run the ATAX implementations across the selected PolyBench datasets.

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
OPENMP_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
ATAX_DIR="${OPENMP_DIR}/linear-algebra/kernels/atax"
RESULTS_DIR="${OPENMP_DIR}/results"

IMPLEMENTATIONS=(
    "atax_seq.c"
    "atax_tmp.c"
    "atax_A.c"
    "atax_partial.c"
)

DATASETS=(
    "MINI_DATASET"
    "SMALL_DATASET"
    "STANDARD_DATASET"
    "LARGE_DATASET"
)

mkdir -p "${RESULTS_DIR}"
LOG_FILE="${RESULTS_DIR}/benchmark_$(date +%Y%m%d_%H%M%S).log"

# Check that the ATAX directory and implementation files exist
if [[ ! -d "${ATAX_DIR}" ]]; then
    echo "Error: ATAX directory not found: ${ATAX_DIR}" >&2
    exit 1
fi

for implementation in "${IMPLEMENTATIONS[@]}"; do
    if [[ ! -f "${ATAX_DIR}/${implementation}" ]]; then
        echo "Error: implementation not found: ${ATAX_DIR}/${implementation}" >&2
        exit 1
    fi
done

echo "ATAX benchmark runs" | tee -a "${LOG_FILE}"
echo "OpenMP default thread configuration" | tee -a "${LOG_FILE}"
echo "Log file: ${LOG_FILE}" | tee -a "${LOG_FILE}"
echo | tee -a "${LOG_FILE}"

# Run every implementation with each dataset
for dataset in "${DATASETS[@]}"; do
    for implementation in "${IMPLEMENTATIONS[@]}"; do
        echo "============================================================" | tee -a "${LOG_FILE}"
        echo "Dataset: ${dataset} | Implementation: ${implementation}" | tee -a "${LOG_FILE}"
        echo "============================================================" | tee -a "${LOG_FILE}"

        make -C "${ATAX_DIR}" \
            "IMPL=${implementation}" \
            "EXT_CFLAGS=-D${dataset} -DPOLYBENCH_TIME" \
            clean all run 2>&1 | tee -a "${LOG_FILE}"

        echo | tee -a "${LOG_FILE}"
    done
done

echo "All benchmark runs completed." | tee -a "${LOG_FILE}"
