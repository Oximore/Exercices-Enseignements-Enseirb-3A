#include "toolsMatrix.h"


void Matrix_initRand(float* a, int size, int min, int max){
    int m = size*size;
  int i;
  for ( i=0 ; i<m ; ++i) {
    a[i] = (float) ((rand() % (max-min)) +min);
  }
}

void Matrix_initNull(float* a, int size){
  int m = size*size;
  int i;
  for ( i=0 ; i<m ; ++i) {
    a[i] = 0.0;
  }
}


void Matrix_mull(float* a, float* b, float* c, int size){
  int off,x,y,k;
  for ( x=0 ; x<size ; ++x){
    for ( y=0 ; y<size ; ++y) {
      off = y*size+x;
      c[off] = 0;
      for ( k=0 ; k<size ; ++k){
	c[off] += a[y*size+k] * b[k*size+x]; 	
      }
    }
  }
}

void Matrix_equivalant(float* a, float* b, int size) {
  int res = Matrix_compare(a,b,size); 
  if (res == -1 )
    printf("Valid Result\n");
  else {
    printf("False Result !\n");
    printf("A[%d] = %f and B[%d] = %f \n",res,a[res],res,b[res]);
  }
}

// return -1 si tout va bien la case si faux
int Matrix_compare(float* a, float* b, int size) {
  float min = 0.1;
  int i,max = size*size;
  for ( i=0 ; i<max ; ++i) {
    if ((a[i]-b[i]) > min)
      return i;
  }
  return -1;
}



void Matrix_display(float* a, int size, int max){
  int i;
  int m =  (max<size*size) ? max : size*size; // prend le plus petit
  // printf("\n");
  for (i=0;i<m;i++){
    if (!(i%size)){
      printf("\n");
    }

    printf("%f\t",a[i]);
  }
  printf("\n");
}
