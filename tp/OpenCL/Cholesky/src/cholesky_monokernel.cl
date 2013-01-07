#include "cholesky.h"
__kernel void cholesky_monokernel(__global MATRIX_TYPE * a) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  // Ce kernel ne marche (de façon déterministe) que lorsque il ne s'exécute sur qu'un work groupe, donc x_loc = x et y = y_loc
  
  // réservation de mémoire locale
  __local float A_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    
  // copy local
  int i,j, my_indice;
  //MATRIX_TYPE aii, aik, aij;
  
  my_indice = x + n*y;
  A_loc[my_indice] = a[my_indice];
  
  // compute
  for ( i=0 ; i<n ; ++i) {
    if (x==i && y==i) {
      A_loc[my_indice] = sqrt(A_loc[my_indice]);
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if (x==i && i < y) {
      A_loc[my_indice] = A_loc[my_indice]/A_loc[i + i*n];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    for ( j=i+1 ; j<n ; ++j) {
      if (y==j &&  i<x  && x<=j) {
	A_loc[my_indice] = A_loc[my_indice] - A_loc[i+x*n]*A_loc[i+j*n];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }
  
  // ecriture du résultat
  a[my_indice] = A_loc[my_indice];
}

