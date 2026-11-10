#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <polybench.h>
#include "atax.h"
#include "atax_validation.h"

/* Save the reference output vector and dataset dimensions to a file */
void save_reference(const char *filename,
                    int nx, int ny,
                    DATA_TYPE *y)
{
  FILE *file = fopen(filename, "w");

  if (file == NULL)
  {
    perror("Error opening reference file");
    exit(EXIT_FAILURE);
  }

  // Write the matrix dimensions
  fprintf(file, "%d %d\n", nx, ny);

  // Write the reference output vector
  for (int i = 0; i < ny; i++)
    fprintf(file, "%.17g\n", (double)y[i]);

  fclose(file);

  printf("Reference saved to %s\n", filename);
}

/* Compare the computed output vector with the reference values */
int check_reference(const char *filename,
                    int nx, int ny,
                    DATA_TYPE *y)
{
  FILE *file = fopen(filename, "r");

  if (file == NULL)
  {
    perror("Error opening reference file");
    return 0;
  }

  int ref_nx, ref_ny;

  // Read and verify the dataset dimensions
  if (fscanf(file, "%d %d", &ref_nx, &ref_ny) != 2 ||
    ref_nx != nx || ref_ny != ny)
  {
    fprintf(stderr, "Reference dataset mismatch!\n");
    fclose(file);
    return 0;
  }

  // Define absolute and relative tolerances
  const double atol = 1e-10;
  const double rtol = 1e-8;

  double max_error = 0.0;
  int errors = 0;

  // Compare each computed element with its reference value
  for (int i = 0; i < ny; i++)
  {
    double reference;

    if (fscanf(file, "%lf", &reference) != 1)
    {
      fprintf(stderr, "Invalid reference file!\n");
      fclose(file);
      return 0;
    }

    double current = (double)y[i];
    
    //fprintf(stderr, "iteration n.%i | current: %.17g - reference: %.17g \n", i, current, reference);
    
    // Check for invalid numerical values
    if (!isfinite(current) || !isfinite(reference))
    {
      errors++;
      continue;
    }
    
    // Compute the absolute difference
    double error = fabs(current - reference);

    if (error > max_error)
      max_error = error;

    // Check the difference against the absolute and relative tolerances
    if (error > atol + rtol * fabs(reference))
      errors++;
  }

  fclose(file);

  // Print the validation result
  if (errors == 0)
  {
    printf("Validation PASSED!\n");
    printf("Maximum absolute error: %.12e\n", max_error);
    return 1;
  }

  printf("Validation FAILED!\n");
  printf("Number of incorrect elements: %d / %d\n", errors, ny);
  printf("Maximum absolute error: %.12e\n", max_error);

  return 0;
}
