
#define PRIMAIRE__CLMULL_NOT

#ifdef PRIMAIRE__CLMULL
// Mull primaire
__kernel void matrix_mull(__global float * a, __global float * b, __global float * c) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int k;
  float result =0;
  
  for ( k=0 ; k<n ; ++k ) {	
    result += a[y*n+k]*b[k*n+x];
  }
  c[y*n + x] = result;
}

#else

__kernel void matrix_mull(__global float * a, __global float * b, __global float * c) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
    
  // réservation de mémoire locale
  __local float A_loc[32*32];
  __local float B_loc[32*32];
  
  // initialisation des offset  
  int off_loc = y_loc*32 + x_loc;
 
  int off_glob_a = y*n + x_loc;
  int off_glob_b = y_loc*n + x;
  //int diff_off = n*32;
  
  int max = n/32;
  int k, l;
  float res = 0;
  for ( k=0 ; k<max ; ++k ) {
     
    // Copie en memoire locale
    A_loc[off_loc] = a[off_glob_a];
    B_loc[off_loc] = b[off_glob_b];
    
    // fin de la copie en memoire locale
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for ( l=0 ; l<32 ; ++l) { 
      // somme des multiplcation du sous groupe
      res += A_loc[y_loc*32 + l]*B_loc[l*32 + x_loc];  
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // calcul des offset suivant
    off_glob_a += 32;
    off_glob_b += n*32;
  }

  c[y*n + x] = res;
}

#endif
