#include "matrixMultiplication.h"

#include "matrixMultiplicationKernel.cu"



#define WA 2048
#define HA WA
#define WB WA
#define HB WA
#define WC WA
#define HC HB

#define BLOCK_SIZE 16



int main(int argc, char ** argv){
  double performance;
  perf_t start,stop;

  long flop = (long)2*WA*WA*WA;


  // C = A * B

  srand(2012);

  // Print informaitons about the system
  print_information();

  // Allocation Memmory for matrix
  float * A = (float *) malloc(sizeof(float)*WA*HA);
  float * B = (float *) malloc(sizeof(float)*WB*HB);
  float * C = (float *) malloc(sizeof(float)*WC*HC);
  float * D = (float *) malloc(sizeof(float)*WC*HC);
  memset(C,0,WC*HC*sizeof(float));
  memset(D,0,WC*HC*sizeof(float));

  // Init matrix
  init_matrix(A,WA,HA);
  init_matrix(B,WB,HB);

  struct timeval tv;
  double time_start;
  double time_end;
  
  double time_gpu;
  double time_cpu;

  // Start Multiplication CPU

  printf("\n\nMultiplication with CPU using openMP\n");
  printf("-----------------------------------\n");

  printf("\nSize Matrix %dx%d\n",WA,WB);



  gettimeofday(&tv,NULL);
  time_start = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("Starting multiplication CPU ...");
  perf(&start);
  matrixMultiplicationOMP(A,WA,HA,B,WB,HB,D,WC,HC);
  perf(&stop);
  gettimeofday(&tv,NULL);
  time_end = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("[OK]\n");
  printf("Time : %f s\n",(float)(time_end-time_start));
  perf_diff(&start, &stop);
  performance = perf_mflops(&stop, flop); 
  printf("Mflop/s : %lf \n", performance);
  time_cpu = time_end-time_start;
 

  printf("\n\nMultiplication without optimisation\n");
  printf("-----------------------------------\n");

  printf("\nSize Matrix %dx%d\n",WA,WB);

  

  // Start Multiplication GPU

  gettimeofday(&tv,NULL);
  time_start = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("\nStarting multiplication GPU ...");
  perf(&start);
  matrixMultiplication(A,WA,HA,B,WB,HB,C,WC,HC);
  perf(&stop);
  gettimeofday(&tv,NULL);
  time_end = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("[OK]\n");
  printf("Time : %f s\n",(float)(time_end-time_start));

  perf_diff(&start, &stop);
  performance = perf_mflops(&stop, flop); 
  printf("Mflop/s : %lf \n", performance);


  time_gpu = time_end-time_start;
  

  if(matrix_compare(C,D,WC*HC)){
    printf("Multiplication correct\n");
  }
  else 
    printf("Multiplication incorrect\n");

  int speed_up = (int)time_cpu/time_gpu;
  printf("Speed UP x%d\n",speed_up);

  memset(C,0,WC*HC*sizeof(float));

  printf("\n\nMultiplication with first optimisation\n");
  printf("-----------------------------------------\n");

  printf("\nSize Matrix %dx%d\n",WA,WB);

  // Start Multiplication GPU

  gettimeofday(&tv,NULL);
  time_start = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("Starting multiplication GPU ...");
  perf(&start);
  matrixMultiplication2(A,WA,HA,B,WB,HB,C,WC,HC);
  perf(&stop);
  gettimeofday(&tv,NULL);
  time_end = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("[OK]\n");
  printf("Time : %f s\n",(float)(time_end-time_start));

  perf_diff(&start, &stop);
  performance = perf_mflops(&stop, flop); 
  printf("Mflop/s : %lf \n", performance);



  time_gpu = time_end-time_start;


  if(matrix_compare(C,D,WC*HC)){
    printf("Multiplication correct\n");
  }
  else 
    printf("Multiplication incorrect\n");

  speed_up = (int)time_cpu/time_gpu;
  printf("Speed UP x%d\n",speed_up);

  memset(C,0,WC*HC*sizeof(float));

  printf("\n\nMultiplication with second optimisation\n");
  printf("-----------------------------------------\n");

  printf("\nSize Matrix %dx%d\n",WA,WB);

  // Start Multiplication GPU

  gettimeofday(&tv,NULL);
  time_start = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("Starting multiplication GPU ...");
  matrixMultiplication3(A,WA,HA,B,WB,HB,C,WC,HC);
  gettimeofday(&tv,NULL);
  time_end = tv.tv_sec+(tv.tv_usec/1000000.0);
  printf("[OK]\n");
  printf("Time : %f s\n",(float)(time_end-time_start));

  time_gpu = time_end-time_start;


  if(matrix_compare(C,D,WC*HC)){
    printf("Multiplication correct\n");
  }
  else 
    printf("Multiplication incorrect\n");

  speed_up = (int)time_cpu/time_gpu;
  printf("Speed UP x%d\n",speed_up);



  //  print_matrix(C,HC,WC);
  //  print_matrix(D,HC,WC);

  free(A);
  free(B);
  free(C);
  free(D);
}


void init_matrix(float * M,int col,int lin){
  int i = 0;
  for(i = 0; i < col * lin; ++i)
    M[i] = rand() / (float)RAND_MAX;
}



void matrixMultiplication(float* A,int wa,int ha,float* B,int wb,int hb,float* C,int wc ,int hc){
  
  float* gpu_A;
  float* gpu_B;
  float* gpu_C;
  unsigned int sizeA = sizeof(float) * wa * ha;
  unsigned int sizeB = sizeof(float) * wb * hb;
  unsigned int sizeC = sizeof(float) * wc * hc;
  cudaMalloc((void**) &gpu_A, sizeA);
  cudaMalloc((void**) &gpu_B, sizeB);
  cudaMemcpy(gpu_A,A,sizeA,cudaMemcpyHostToDevice);
  cudaMemcpy(gpu_B,B,sizeB,cudaMemcpyHostToDevice);
  cudaMalloc((void**) &gpu_C, sizeC);

  dim3 threads(32,32);
  dim3 grid( wc / threads.x, hc / threads.y);
  cudaThreadSynchronize();
  matrixMul<<< grid, threads >>>(gpu_C,gpu_A,gpu_B,wa,wb);
  cudaThreadSynchronize();
  cudaMemcpy(C,gpu_C,sizeC,cudaMemcpyDeviceToHost);
  cudaThreadSynchronize();

  cudaFree(gpu_A);
  cudaFree(gpu_B);
  cudaFree(gpu_C);
}


void matrixMultiplication2(float* A,int wa,int ha,float* B,int wb,int hb,float* C,int wc ,int hc){
  
  float* gpu_A;
  float* gpu_B;
  float* gpu_C;
  unsigned int sizeA = sizeof(float) * wa * ha;
  unsigned int sizeB = sizeof(float) * wb * hb;
  unsigned int sizeC = sizeof(float) * wc * hc;
  cudaMalloc((void**) &gpu_A, sizeA);
  cudaMalloc((void**) &gpu_B, sizeB);
  cudaMemcpy(gpu_A,A,sizeA,cudaMemcpyHostToDevice);
  cudaMemcpy(gpu_B,B,sizeB,cudaMemcpyHostToDevice);
  cudaMalloc((void**) &gpu_C, sizeC);

  dim3 threads(BLOCK_SIZE,BLOCK_SIZE);
  dim3 grid( wc / threads.x, hc / threads.y);
  cudaThreadSynchronize();
  matrixMul2<<< grid, threads >>>(gpu_C,gpu_A,gpu_B,wa,wb);
  cudaThreadSynchronize();
  cudaMemcpy(C,gpu_C,sizeC,cudaMemcpyDeviceToHost);
  cudaThreadSynchronize();

  cudaFree(gpu_A);
  cudaFree(gpu_B);
  cudaFree(gpu_C);
}



void matrixMultiplication3(float* A,int wa,int ha,float* B,int wb,int hb,float* C,int wc ,int hc){
  
  float* gpu_A;
  float* gpu_B;
  float* gpu_C;
  unsigned int sizeA = sizeof(float) * wa * ha;
  unsigned int sizeB = sizeof(float) * wb * hb;
  unsigned int sizeC = sizeof(float) * wc * hc;
  cudaMalloc((void**) &gpu_A, sizeA);
  cudaMalloc((void**) &gpu_B, sizeB);
  cudaMemcpy(gpu_A,A,sizeA,cudaMemcpyHostToDevice);
  cudaMemcpy(gpu_B,B,sizeB,cudaMemcpyHostToDevice);
  cudaMalloc((void**) &gpu_C, sizeC);

  dim3 threads(16,16);
  dim3 grid( wc / threads.x, hc / threads.y);
  cudaThreadSynchronize();
  matrixMul3<<< grid, threads >>>(gpu_C,gpu_A,gpu_B,wa,wb);
  cudaThreadSynchronize();
  cudaMemcpy(C,gpu_C,sizeC,cudaMemcpyDeviceToHost);
  cudaThreadSynchronize();

  cudaFree(gpu_A);
  cudaFree(gpu_B);
  cudaFree(gpu_C);
}

bool matrix_compare(float *a, float *b,int wc){
  int i = 0;
  for(i = 0;i < wc;++i){
    if( a[i] - b[i] > 0.1 || a[i] - b[i] < -0.1 ){
      return false;  
    }
  }
  return true;
}

void matrixMultiplicationOMP(float* A,int wa,int ha,float* B,int wb,int hb,float* C,int wc ,int hc){
  int i,j,k;
  int n = wa;
  omp_set_num_threads(8);

#pragma omp parallel for default(none) shared(A,B,C,wa,n) private(k,j)
  for (i=0; i<wa; ++i)
    {
       for(j=0; j<wa; ++j)       
	 {
            for (k=0; k<wa; ++k)
	      {
                 C[i*wa+j] += A[i*n+k] * B[k*n+j];
              }
         }
    }
}





void matrixMultiplicationSlow(float* A,int wa,int ha,float* B,int wb,int hb,float* C,int wc ,int hc){
  int n = wa;
  int i,j,k;
  for(i = 0 ; i < n ; ++i){
    for(j = 0 ; j < n ; ++j){
      for(k=0 ; k < n  ; ++k){
	C[i*n+j] = C[i*n+j] +  A[i*n+k]*B[k*n+j];
      }
    }
  }


}


void print_matrix(float* A,int lin,int col){
 printf("\n\nMatrix\n");
 for(int i = 0; i < lin*col; i++)
    {
      printf("%f ", A[i]);
      if(((i + 1) % col) == 0)
	printf("\n");
    }
 printf("\n");
 
}


void print_information(){



  struct cudaDeviceProp * cudaInfos = (struct cudaDeviceProp *)  malloc(sizeof(struct cudaDeviceProp));
  int number_of_device;
  cudaGetDeviceCount(&number_of_device);
  printf("System Informations\n");
  printf("-------------------\n\n");
  printf("Number of devices : %d\n",number_of_device);
  int device;
  cudaGetDevice(&device);
  cudaGetDeviceProperties(cudaInfos,0);
  
  
  printf("Device Informations\n");
  printf("  Name : %s\n",cudaInfos->name);
  printf("  Memory : %li\n",cudaInfos->totalGlobalMem);
  printf("  WarpSize : %d\n",cudaInfos->warpSize);
  printf("  Max Threads Per Block : %d\n",cudaInfos->maxThreadsPerBlock);
  printf("  Multi processor count : %d\n",cudaInfos->multiProcessorCount);


  cudaThreadSynchronize();
  //  free(cudaInfos);
}
void 
perf(perf_t * p) {
  gettimeofday(p, NULL);  
}

void 
perf_diff(const perf_t * begin, perf_t * end) {
  end->tv_sec = end->tv_sec - begin->tv_sec;
  end->tv_usec = end->tv_usec - begin->tv_usec;
  if (end->tv_usec < 0) {
    (end->tv_sec)--;
    end->tv_usec += 1000000;
  }
}

void
perf_printh(const perf_t * p) {
  long m = p->tv_sec / 60;
  long s = p->tv_sec - m*60;
  long ms = p->tv_usec / 1000;
  long micros = p->tv_usec - ms*1000;

  //  printf("%ld sec %ld usec\n", p->tv_sec, p->tv_usec);
  printf("%ld:%ld:%ld:%ld\n",m,s,ms,micros);
}

void
perf_printmicro(const perf_t * p) {
  printf("%ld\n",p->tv_usec + ( p->tv_sec * 1000000) );
}


double
perf_mflops(const perf_t * p, const long nb_op) {
  return (double)nb_op / (p->tv_sec * 1000000 + p->tv_usec);
}
