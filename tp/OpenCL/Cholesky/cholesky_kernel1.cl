#define MATRIX_SIZE 16

__kernel void cholesky_mid(__global float * a) {
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
  for ( i=0 ; i<matrix_size ; ++i) {
    if (x_loc==i && y_loc==i)
      A_loc[my_local_indice] = sqrt(A_loc[my_local_indice]);
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if (x_loc==i && i < y_loc) {
      A_loc[my_local_indice] = A_loc[my_local_indice]/A_loc[i + i*matrix_size];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    for ( j=i+1 ; j<matrix_size ; ++j) {
      // for ( k=i+1 ; k<=j ; ++k) {
      if (y_loc==j &&  i<x_loc  && x_loc<=j) {
	A_loc[my_local_indice] = A_loc[my_local_indice] - A_loc[i + x_loc*matrix_size]*A_loc[i + j*matrix_size];
      }
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  
  // ecriture du résultat
  a[x + n*y] =  A_loc[my_local_indice];
}

