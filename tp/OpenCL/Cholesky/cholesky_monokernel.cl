#include "cholesky.h"


__kernel void cholesky_monokernel(__global MATRIX_TYPE * a) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
  int n_loc = get_local_size(0);
  
  // réservation de mémoire locale
  __local float A_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    
  // copy local
  int i,j, my_local_indice, my_global_indice;
  MATRIX_TYPE aii, aik, aij;
  
  my_local_indice = x_loc + n_loc*y_loc;
  my_global_indice = x + n*y;
  
  //a[my_global_indice] = sqrt(a[my_global_indice]);
  
  A_loc[my_local_indice] = a[my_global_indice];
  
  // compute
  for ( i=0 ; i<n ; ++i) {
    if (x==i && y==i) {
      A_loc[my_local_indice] = sqrt(A_loc[my_local_indice]);
      a[my_global_indice] = A_loc[my_local_indice];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if (x==i && i < y) {
      aii = a[i + i*n];
      A_loc[my_local_indice] = A_loc[my_local_indice]/aii;
      a[my_global_indice] = A_loc[my_local_indice];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    for ( j=i+1 ; j<n ; ++j) {
      // for ( k=i+1 ; k<=j ; ++k) {
      if (y==j &&  i<x  && x<=j) {
	aik = a[i + x*n];
	aij = a[i + j*n];
	A_loc[my_local_indice] = A_loc[my_local_indice] - aik*aij;
      }
      a[my_global_indice] = A_loc[my_local_indice];
      barrier(CLK_LOCAL_MEM_FENCE);
    }
    
    //barrier(CLK_LOCAL_MEM_FENCE);
  }
  
  // ecriture du résultat
  a[my_global_indice] = A_loc[my_local_indice];
  //a[x + n*y] =  A_loc[my_local_indice];
  //*/
}

