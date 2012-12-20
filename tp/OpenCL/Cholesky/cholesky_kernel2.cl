#define MATRIX_SIZE 16

__kernel void cholesky_inf(__global float * a, __global float * aii) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
    
  int matrix_size = MATRIX_SIZE;
  // réservation de mémoire locale
  __local float A_loc[matrix_size*matrix_size];

  // copy local
  int i,j,k, my_local_indice;
  
  my_local_indice = x_loc + matrix_size*y_loc;
  A_loc[my_local_indice] = a[x + n*y];
  
  // compute
  A_loc[my_local_indice] = A_loc[my_local_indice]/aii ;
  a[x + n*y] =  A_loc[my_local_indice];
}
