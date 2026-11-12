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

/* Parallelization of the ATAX kernel by distributing the computation across rows and columns. */
// col/row switch of matrix A
static void kernel_atax(int nx, int ny,
                        DATA_TYPE POLYBENCH_2D(A, NX, NY, nx, ny),
                        DATA_TYPE POLYBENCH_1D(x, NY, ny),
                        DATA_TYPE POLYBENCH_1D(y, NY, ny),
                        DATA_TYPE POLYBENCH_1D(tmp, NX, nx))
{
  int i, j;
  
  #pragma omp parallel private(i,j)
  {
    // Compute tmp = A * x in parallel
    // Each iteration processes an independent row of A
    
    #pragma omp for
    for (i = 0; i < _PB_NX; i++)
    {
      tmp[i] = 0;
      
      for (j = 0; j < _PB_NY; j++)
        tmp[i] = tmp[i] + A[i][j] * x[j];
    }
    
    // Compute y = A^T * tmp in parallel
    // Each iteration computes a distinct element of y

    // #pragma omp for
    // for (j = 0; j < _PB_NY; j++)
    // {
    //   y[j] = 0;
    //
    //   for (i = 0; i < _PB_NX; i++)
    //     y[j] += A[i][j] * tmp[i];
    // }

    /* PROBLEMA: nella versione sopra (commentata) il loop interno scorre i,
       quindi A[i][j] viene letta per colonne (stride di NY elementi): quasi
       ogni accesso causa un cache miss e la fase 2 è molto più lenta della 1.
       SOLUZIONE: si mantiene l'accesso per righe (i esterno, j interno, stride 1)
       e si parallelizza su i. Thread diversi aggiornerebbero gli stessi y[j],
       quindi si usa una reduction sull'intero array y: ogni thread accumula
       in una copia privata di y e alla fine OpenMP somma le copie in y. */

    // y deve essere azzerato prima: la reduction somma i parziali al valore iniziale
    #pragma omp for
    for (j = 0; j < _PB_NY; j++)
      y[j] = 0;
    // barriera implicita: y è azzerato prima dell'inizio della reduction

    #pragma omp for reduction(+: y[0:_PB_NY])
    for (i = 0; i < _PB_NX; i++)
    {
      for (int jj = 0; jj < _PB_NY; jj++)   // contatore locale -> privato per thread
        y[jj] += A[i][jj] * tmp[i];
    }
  }
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
