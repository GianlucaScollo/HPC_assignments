#ifndef ATAX_VALIDATION_H
#define ATAX_VALIDATION_H

#include "atax.h"

void save_reference(const char *filename, int nx, int ny, DATA_TYPE *y);
int check_reference(const char *filename, int nx, int ny, DATA_TYPE *y);

#endif
