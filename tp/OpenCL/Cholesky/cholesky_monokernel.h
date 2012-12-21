#ifndef CHOLESKY_MONOKERNEL_H
#define CHOLESKY_MONOKERNEL_H

#include "cholesky.h"

#define bool int
#define true 1
#define false 0

#define CHECK_ERROR(a,b) if (a!=CL_SUCCESS)\
    printf("Error in %s.\n",b)


//void CreateKernel(cl_kernel* kernel, cl_context ctx, const char* file_name, const char* function_name,bool verbose);
void CreateKernel(cl_kernel* kernel, cl_context ctx,   cl_device_id* devs, cl_uint nb_devs, const char* file_name, const char* function_name,bool verbose) ;
void DisplayMatrix(MATRIX_TYPE* mat, unsigned int size);
void InitMatrix(MATRIX_TYPE * matA, unsigned int matrix_size);
void InitMatrix2(MATRIX_TYPE * matA, unsigned int matrix_size);

void MullMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, MATRIX_TYPE * c, unsigned int size) ;
void TransposeMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size) ;
void ClearUpMatrix(MATRIX_TYPE * a, unsigned int size) ;
void MinusMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size);


#endif
