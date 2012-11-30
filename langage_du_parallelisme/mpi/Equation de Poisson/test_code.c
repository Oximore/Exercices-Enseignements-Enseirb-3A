#include <stdio.h>
#include <stdlib.h>

#include "test_code.h"


double f(double x, double y){
  return -2*(x*(x-1) + y*(y-1));
}

double solution(double x, double y){
  return x*(x-1)*y*(y-1);
}

void print_matrix(double* m, int n) {

  int i,j;
  for ( j=0 ; j<n ; ++j){
    for ( i=0 ; i<n ; ++i){
      printf("%lf\t",m[i + n*j]);
    }
    printf("\n");
  }
}


