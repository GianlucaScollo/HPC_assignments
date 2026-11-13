#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "atax.h"

/* Include utils-function header. */
#include "atax_validation.h"

/* Include omp */
#include <omp.h>

/* Array initialization. */
static void init_array(int nx, int ny,
                       DATA_TYPE POLYBENCH_2D(A, NX, NY, nx, ny),
                       DATA_TYPE POLYBENCH_1D(x, NY, ny))
{
  int i, j;
  
  // Initialize the input vector x
  for (i = 0; i < ny; i++)
    x[i] = i * M_PI;
    
  // Initialize the matrix A
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      A[i][j] = ((DATA_TYPE)i * (j + 1)) / nx;
}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(int nx,
                        DATA_TYPE POLYBENCH_1D(y, NX, nx))

{
  int i;

  for (i = 0; i < nx; i++)
  {
    fprintf(stderr, DATA_PRINTF_MODIFIER, y[i]);
    if (i % 20 == 0)
      fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

/* Main computational kernel. The whole function will be timed,
   including the call and return. */

/* Parallelization of the ATAX kernel using thread-private partial output vectors */
static void kernel_atax(int nx, int ny,
                        DATA_TYPE POLYBENCH_2D(A, NX, NY, nx, ny),
                        DATA_TYPE POLYBENCH_1D(x, NY, ny),
                        DATA_TYPE POLYBENCH_1D(y, NY, ny),
                        DATA_TYPE POLYBENCH_1D(tmp, NX, nx))
{
  // Get the maximum number of threads and initialize the actual thread count
  int nthreads = omp_get_max_threads();
  int actual_threads = nthreads;

  // Allocate one zero-initialized partial output vector for each thread
  DATA_TYPE *partial_y = calloc((size_t)nthreads * ny, sizeof(DATA_TYPE));

  if (partial_y == NULL)
  {
    fprintf(stderr, "Error allocating partial vectors\n");
    exit(EXIT_FAILURE);
  }

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();

    // Get the partial output vector assigned to the current thread
    DATA_TYPE *local_y = partial_y + (size_t)tid * ny;

    // Get the actual number of threads in the parallel region
    #pragma omp single
    actual_threads = omp_get_num_threads();

    // Compute tmp = A * x and accumulate partial results for y
    #pragma omp for
    for (int i = 0; i < _PB_NX; i++)
    {
      tmp[i] = 0;

      // Compute tmp[i] as the dot product of row i of A and vector x
      for (int j = 0; j < _PB_NY; j++)
        tmp[i] += A[i][j] * x[j]; 

      // Accumulate the contribution of row i into the thread-private vector
      for (int j = 0; j < _PB_NY; j++)
        local_y[j] += A[i][j] * tmp[i];
    }

    // Combine the partial vectors to obtain the final output y
    #pragma omp for
    for (int j = 0; j < _PB_NY; j++)
    {
      y[j] = 0;

      // Sum the contributions from all threads for output element j
      for (int t = 0; t < actual_threads; t++)
        y[j] += partial_y[(size_t)t * ny + j];
    }
  }

  // Release the memory allocated for the partial output vectors
  free(partial_y);
}

int main(int argc, char **argv)
{
  /* Retrieve problem size. */
  int nx = NX;
  int ny = NY;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, NX, NY, nx, ny);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(tmp, DATA_TYPE, NX, nx);

  /* Initialize array(s). */
  init_array(nx, ny, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(x));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_atax(nx, ny,
              POLYBENCH_ARRAY(A),
              POLYBENCH_ARRAY(x),
              POLYBENCH_ARRAY(y),
              POLYBENCH_ARRAY(tmp));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;
  
  check_reference(ATAX_REFERENCE_PATH, nx, ny, POLYBENCH_ARRAY(y));
  
  //print_array(nx, POLYBENCH_ARRAY(y));
  
  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(nx, POLYBENCH_ARRAY(y)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(y);
  POLYBENCH_FREE_ARRAY(tmp);

  return 0;
}
