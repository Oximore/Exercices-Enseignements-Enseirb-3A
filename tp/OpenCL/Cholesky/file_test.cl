
__kernel void test(__global float * a) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int n = get_global_size(0);
  
  a[x + n*y ] = 3;
}

