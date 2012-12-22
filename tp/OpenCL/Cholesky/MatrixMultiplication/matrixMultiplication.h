#ifndef MATRIXMULTIPLICATION_H
#define MATRIXMULTIPLICATION_


/* System */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
/* Cuda */
#include <cuda.h>
#include <curand.h>
#include <cutil.h>
#include <omp.h>



void init_matrix(float *,int,int);
void matrixMultiplication(float*,int,int,
			  float*,int,int,
			  float*,int,int);
void matrixMultiplication2(float*,int,int,
			  float*,int,int,
			  float*,int,int);

void matrixMultiplication3(float*,int,int,
			  float*,int,int,
			  float*,int,int);
	  
void matrixMultiplicationSlow(float*,int,int,
			  float*,int,int,
			  float*,int,int);

void matrixMultiplicationOMP(float*,int,int,
			  float*,int,int,
			  float*,int,int);


bool matrix_compare(float*, float*,int);
void print_matrix(float*,int,int);

void print_information();


#include <sys/time.h>

typedef struct timeval perf_t;

void 
perf(perf_t * p);

void 
perf_diff(const perf_t * begin, perf_t * end);

void
perf_printh(const perf_t * p);

void
perf_printmicro(const perf_t * p);

double
perf_mflops(const perf_t * p, const long nb_op);


#endif
