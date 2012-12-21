#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#include "cholesky.h"

/*** Defines ***/
#define bool int
#define true 1
#define false 0

#define CHECK_ERROR(a,b) if (a!=CL_SUCCESS)\
    printf("Error in %s.\n",b)

/*** Prototypes ***/
void InitMatrix(MATRIX_TYPE * matA, unsigned int matrix_size);
void InitMatrix2(MATRIX_TYPE * matA, unsigned int matrix_size);
void InitMatrix3(MATRIX_TYPE * matA, unsigned int matrix_size);

void DisplayMatrix(MATRIX_TYPE* mat, unsigned int size);

void ClearUpMatrix(MATRIX_TYPE * a, unsigned int size) ;
void TransposeMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size) ;
void MullMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, MATRIX_TYPE * c, unsigned int size) ;
void MinusMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size);

#endif
