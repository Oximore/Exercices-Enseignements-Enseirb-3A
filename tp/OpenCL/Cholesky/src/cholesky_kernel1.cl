#include "cholesky.h"

__kernel void cholesky_diag(__global MATRIX_TYPE * a, int indice) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
  int n_loc = get_local_size(0);
  // impose bloc carré
  

  // Si je suis le kernel de l'étape indice
  if (indice == (x-x_loc)/n_loc
      && indice == (y-y_loc)/n_loc) {
    int i,j, my_local_indice, my_global_indice;
    
    // réservation de mémoire locale
    __local MATRIX_TYPE A_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    
    // copy local
    my_local_indice = x_loc + n_loc*y_loc;
    my_global_indice = x + n*y;
    
    A_loc[my_local_indice] = a[my_global_indice];
    
    // compute
    
    for ( i=0 ; i<n_loc ; ++i) {
      if (x_loc==i && y_loc==i) {
	A_loc[my_local_indice] = sqrt(A_loc[my_local_indice]);
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      
      if (x_loc==i && i < y_loc) {
	A_loc[my_local_indice] = A_loc[my_local_indice]/A_loc[i + i*n_loc];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      
      for ( j=i+1 ; j<n ; ++j) {
	// for ( k=i+1 ; k<=j ; ++k) {
	if (y_loc==j &&  i<x_loc  && x_loc<=j) {
	  A_loc[my_local_indice] = A_loc[my_local_indice] - A_loc[i + x_loc*n_loc]*A_loc[i + j*n_loc];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
      }
    }
    //*/

    // ecriture du résultat
    a[my_global_indice] = A_loc[my_local_indice];    
    

    //    if (indice==1)
    // a[my_global_indice]=my_local_indice;
  }
}

