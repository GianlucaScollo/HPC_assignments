#ifndef ATAX_H
# define ATAX_H

/* Default to STANDARD_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(STANDARD_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define STANDARD_DATASET
# endif

/* Do not define anything if the user manually defines the size. */
# if !defined(NX) && !defined(NY)
/* Define the possible dataset sizes. */
#  ifdef MINI_DATASET
#   define NX 32
#   define NY 32
#   define ATAX_REFERENCE_PATH "References/atax_mini.ref"
#  endif

#  ifdef SMALL_DATASET
#   define NX 500
#   define NY 500
#   define ATAX_REFERENCE_PATH "References/atax_small.ref"
#  endif

#  ifdef STANDARD_DATASET /* Default if unspecified. */
#   define NX 4000
#   define NY 4000
#   define ATAX_REFERENCE_PATH "References/atax_standard.ref"
#  endif

#  ifdef LARGE_DATASET
#   define NX 8000
#   define NY 8000
#   define ATAX_REFERENCE_PATH "References/atax_large.ref"
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NX 100000
#   define NY 100000
#   define ATAX_REFERENCE_PATH "References/atax_extralarge.ref"
#  endif
# endif /* !N */

# define _PB_NX POLYBENCH_LOOP_BOUND(NX,nx)
# define _PB_NY POLYBENCH_LOOP_BOUND(NY,ny)

# ifndef DATA_TYPE
#  define DATA_TYPE double
#  define DATA_PRINTF_MODIFIER "%0.2lf "
# endif


#endif /* !ATAX */
