#include "test.h"

__kernel void test(__global float * a, int value) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
 
  int x_loc = get_local_id(0);
  int y_loc = get_local_id(1);
  int x_max_loc = get_local_size(0);
  int y_max_loc = get_local_size(1);
  

  // copy local
  int my_local_indice;
  __local float A_loc[8*8];
  
  my_local_indice = x_loc + x_max_loc*y_loc;
  
  //A_loc[my_local_indice] = a[x + n*y];

  //A_loc[my_local_indice] = my_local_indice;
  A_loc[my_local_indice] = value;
  

  a[x + n*y] = A_loc[my_local_indice];
 
}
