
#ifndef TOOLSMATRIX_H
#define TOOLSMATRIX_H 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Matrix_initNull(float* a, int size);
void Matrix_initRand(float* a, int size, int min, int max);

void Matrix_mull(float* a, float* b, float* c, int size);

void Matrix_equivalant(float* a, float* b, int size);
int Matrix_compare(float* a, float* b, int size);

void Matrix_display(float* a, int size, int max);

#endif
