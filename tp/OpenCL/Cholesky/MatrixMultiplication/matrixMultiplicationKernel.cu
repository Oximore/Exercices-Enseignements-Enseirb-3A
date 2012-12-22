#ifndef MATRIXMULTIPLICATIONKERNEL_CU
#define MATRIXMULTIPLICATIONKERNEL_CU

#include <curand.h>


__global__ void matrixMul(float * g_C, float * g_A, float *g_B,int wa, int wb){


  int x = blockIdx.x * blockDim.y + threadIdx.x;
  int y = blockIdx.y*blockDim.x + threadIdx.y;
  
  float result = 0;

  int i = 0;
  for(i = 0; i < wa; ++i){
    float tempA = g_A[y*wa+i];
    float tempB = g_B[i*wb+x];
    result += tempA*tempB;
  }

  g_C[y*wa+x] = result;

}


__global__ void matrixMul2(float * g_C, float * g_A, float *g_B,int wa, int wb){
  int TILE_SIZE = 16;

  int x = blockIdx.x*TILE_SIZE + threadIdx.x;
  int y = blockIdx.y*TILE_SIZE + threadIdx.y;
  
  float result = 0;

  int i = 0;
  for(i = 0; i < wa; ++i){
    float tempA = g_A[y*wa+i];
    float tempB = g_B[i*wb+x];
    result += tempA*tempB;
  }

  g_C[y*wa+x] = result;

}



__global__ void matrixMul3(float * g_C, float * g_A, float *g_B,int wa, int wb){
  const int TILE_WIDTH = 16;
  int tx = threadIdx.x;
  int ty = threadIdx.y;
  int bx = blockIdx.x;
  int by = blockIdx.y;

  __shared__ float s_a[TILE_WIDTH][TILE_WIDTH];
  __shared__ float s_b[TILE_WIDTH][TILE_WIDTH];

  int row = bx*blockDim.y + tx;
  int col = by*blockDim.x + ty;



  float result = 0;

  int i = 0;
  for(i = 0; i < wa/TILE_WIDTH; ++i){
    s_a[tx][ty] = g_A[i*TILE_WIDTH + row*wa +ty];
    s_b[tx][ty] = g_B[(i*TILE_WIDTH*wa)+tx*wa+ col];
    __syncthreads();

    int k =0;
    for(k=0;k<TILE_WIDTH;++k){
      result += s_a[tx][k] * s_b[k][ty];
    }
    __syncthreads();
  }

  g_C[row*wa+col] = result;

}



#endif
