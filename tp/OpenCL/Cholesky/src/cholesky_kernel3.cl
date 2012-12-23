#include "cholesky.h"

__kernel void cholesky_subdiag(__global MATRIX_TYPE * a, int indice) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
  int n_loc = get_local_size(0);


  // Si je suis un des kernels 3 de l'étape indice
  if (indice < (x-x_loc)/n_loc
      && (x-x_loc)/n_loc <= (y-y_loc)/n_loc) {
    int i, my_local_indice, my_global_indice, a_global_indice, b_global_indice;
    MATRIX_TYPE tmp_value;

    // réservation de mémoire locale
    __local MATRIX_TYPE M_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    __local MATRIX_TYPE A_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
    __local MATRIX_TYPE B_loc[LOCAL_DIM_KERNEL*LOCAL_DIM_KERNEL];
        
    // copy local
    my_local_indice = x_loc + n_loc*y_loc;
    my_global_indice = x + n*y;
    a_global_indice = (x_loc + n_loc*indice) + (x-x_loc + y_loc)*n;
    b_global_indice = (x_loc + n_loc*indice) + y*n;
    
    M_loc[my_local_indice] = a[my_global_indice];
    A_loc[my_local_indice] = a[a_global_indice];
    B_loc[my_local_indice] = a[b_global_indice];
    
    // compute
    barrier(CLK_LOCAL_MEM_FENCE);
    tmp_value = M_loc[my_local_indice];
    for (i=0 ; i<n_loc ; i++){
      tmp_value -= A_loc[i + y_loc*n_loc]*B_loc[i + x_loc*n_loc];
    }
    //*/
    a[my_global_indice] = tmp_value; 
    //a[my_global_indice] = a_global_indice; 
    
  }
}
