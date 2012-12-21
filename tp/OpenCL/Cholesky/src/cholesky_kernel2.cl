#include "cholesky.h"

__kernel void cholesky_inf(__global MATRIX_TYPE * a, int indice) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
  int n_loc = get_local_size(0);


  // Si je suis le kernel de l'étape indice
  if (indice == (x-x_loc)/n_loc
      && indice < (y-y_loc)/n_loc) {
    int i, my_local_indice, my_global_indice, diag_global_indice;
    

    // réservation de mémoire locale
    __local MATRIX_TYPE Diag_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    __local MATRIX_TYPE A_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    
    // copy local
    my_local_indice = x_loc + n_loc*y_loc;
    my_global_indice = x + n*y;
    diag_global_indice = x + n*(indice+y_loc);
    
    A_loc[my_local_indice]    = a[my_global_indice];
    Diag_loc[my_local_indice] = a[diag_global_indice];
    

    // compute
    for (i=0 ; i<n_loc ; i++){
      barrier(CLK_LOCAL_MEM_FENCE);
      // Divise la colone en cours
      if (x_loc==i) { 
	A_loc[my_local_indice] = A_loc[my_local_indice] / Diag_loc[i + i*n_loc];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      // On met à jour le reste
      if (x_loc>i/* && y>=x*/){
	A_loc[my_local_indice] = A_loc[my_local_indice] - Diag_loc[i + x_loc*n_loc]*A_loc[i + y_loc*n_loc];
      }
    }
    
    a[my_global_indice] = A_loc[my_local_indice]; 
  }
}
