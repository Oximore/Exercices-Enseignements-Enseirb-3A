#include "util.h"

void DisplayMatrix(MATRIX_TYPE* mat, unsigned int size){
  unsigned int i,j;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      printf("%lf|"/*(%d|%d)\t"*/,mat[i+j*size]/*,i,j*/);
    }
    printf("\n");
  }
}


void InitMatrix(MATRIX_TYPE * matA, unsigned int matrix_size) {
  unsigned int i;
  for (i=0 ; i<matrix_size*matrix_size ; i++){
    matA[i] = i+1;
  }
}

void InitMatrix3(MATRIX_TYPE * matA, unsigned int matrix_size) {
  unsigned int i,j;
  for (j=0 ; j<matrix_size ; j++) { 
    for (i=0 ; i<matrix_size ; i++){
      matA[i+j*matrix_size] = 15000*1.0/((float)i+(float)j+1);
    }
  }
}

void InitMatrix2(MATRIX_TYPE * matA, unsigned int matrix_size) {
  int i=0;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=5;
  matA[i++]=5;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=14;
  matA[i++]=14;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=14;
  matA[i++]=15;
}

void ClearUpMatrix(MATRIX_TYPE * a, unsigned int size) {
  unsigned int i,j;
   for (j=0 ; j<size ; j++){
     for (i=0 ; i<size ; i++){
       if (i>j)
	 a[i+j*size] = 0;
     }
  }
}

 void TransposeMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size) {
  unsigned int i,j;
   for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      b[i+j*size] = a[j + i*size];
    }
  }
}

void MullMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, MATRIX_TYPE * c, unsigned int size) {
  unsigned int i,j,k;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      c[i+j*size]=0;
      for (k=0 ; k<size ; k++) {
	c[i+j*size] += a[k + j*size]*b[i + k*size];
      }
    }
  }
} 

void MinusMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size){
  unsigned int i,j;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      a[i+j*size] -= b[i+j*size];
    }
  }
}

  
