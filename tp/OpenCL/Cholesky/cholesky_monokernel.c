#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <string.h>
#include <unistd.h>

#include "cholesky_monokernel.h"
#include <sys/time.h>
#include <time.h>


int main(int argc, char* argv[]) {
  
  /* int valeur_max = 2; */
  /* if (argc > 1) */
  /*   valeur_max = atoi(argv[1]);  */

  //  unsigned int i,j;
  cl_int cl_error;
  
  /* struct timeval tv; */
  /* double time_start; */
  /* double time_end; */
  /* srand(time(NULL)); */
   
  unsigned int matrix_size = MATRIX_SIZE;
  unsigned int matrix_total_size = matrix_size*matrix_size;
  size_t cl_buff_size = matrix_total_size * sizeof(MATRIX_TYPE);
  
  printf("Matrix size : %d (%d length)\n",matrix_size,matrix_total_size);
  
  
  // Allouer des buffer non contigues ?
  cl_event ev_writeA, ev_ker, ev_readA;
  
  
  MATRIX_TYPE * matA = malloc(matrix_total_size * sizeof(*matA));
  MATRIX_TYPE * matB = malloc(matrix_total_size * sizeof(*matB));
  MATRIX_TYPE * matC = malloc(matrix_total_size * sizeof(*matC));
  MATRIX_TYPE * matD = malloc(matrix_total_size * sizeof(*matD));
    
  /* gettimeofday(&tv, NULL); */
  /* time_start=tv.tv_sec+(tv.tv_usec/1000000.0); */

  // Init matA
  InitMatrix2(matA, matrix_size);
  //  InitMatrix(matB, matrix_size);
  
  /* gettimeofday(&tv, NULL); */
  /* time_end=tv.tv_sec+(tv.tv_usec/1000000.0); */
  /* printf("%.6lf seconds elapsed for Generate matrix\n", time_end-time_start) */;  
  
  printf("Matrix A:\n");
  DisplayMatrix(matA,matrix_size);
  
  
  // Init GPU
  cl_uint nb_platf;
  clGetPlatformIDs(0, NULL, &nb_platf);
  
  printf("Nombre de plateformes: %d\n", nb_platf);
  
  cl_platform_id platfs[nb_platf];
  clGetPlatformIDs(nb_platf, platfs, NULL);
  
  size_t plat_name_size;
  clGetPlatformInfo(platfs[0], CL_PLATFORM_NAME, 0, NULL, &plat_name_size);
  char plat_name[plat_name_size];
  clGetPlatformInfo(platfs[0], CL_PLATFORM_NAME, plat_name_size, &plat_name, NULL);
  printf("Nom de la plateforme: %s\n", plat_name);

  size_t plat_vendor_size;
  clGetPlatformInfo(platfs[0], CL_PLATFORM_VENDOR, 0, NULL, &plat_vendor_size);
  char plat_vendor[plat_vendor_size];
  clGetPlatformInfo(platfs[0], CL_PLATFORM_VENDOR, plat_vendor_size, &plat_vendor, NULL);
  printf("Nom de la plateforme: %s\n", plat_vendor);

  cl_uint nb_devs;
  clGetDeviceIDs(platfs[0], CL_DEVICE_TYPE_ALL, 0, NULL, &nb_devs);

  cl_device_id devs[nb_devs];
  clGetDeviceIDs(platfs[0], CL_DEVICE_TYPE_ALL, nb_devs, devs, NULL);

  cl_context ctx = clCreateContext(NULL, nb_devs, devs, NULL, NULL, NULL);
  cl_command_queue command_queue = clCreateCommandQueue(ctx, devs[0], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, NULL);


  /*** Chargement du kernel cholesky_monokernel ***/
  cl_kernel ker;
  bool verbose = false;
  //  CreateKernel(&ker,true);
  FILE * f = fopen("./cholesky_monokernel.cl", "r");
  fseek(f, 0, SEEK_END);
  size_t source_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char * source = malloc(source_size+1);
  fread(source, 1, source_size, f);
  source[source_size] = '\0';
  fclose(f);

  if (verbose)
    printf("Code kernel: %s\n", source);

  char buildOption[512] = "-I ";
  char* current_directory = get_current_dir_name();
  strcat(buildOption,current_directory);
  strcat(buildOption,"/");
  free(current_directory);
  
  if (verbose)
    printf("BUILD OPTION : %s\n\n",buildOption);
  
  cl_program prg = clCreateProgramWithSource(ctx, 1, (const char**)&source, NULL, NULL);
  clBuildProgram(prg, nb_devs, devs, buildOption, NULL, NULL);


  size_t log_size;
  clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
  char log[log_size];
  clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
 
  //  if (verbose)
  printf("Build log: %s\n", log);

  // Compilation du kernel
  ker = clCreateKernel(prg, "cholesky_monokernel", &cl_error);
  CHECK_ERROR(cl_error,"clCreateBuffer");


  // Création des buffers
  cl_mem bufA = clCreateBuffer(ctx, CL_MEM_READ_WRITE, cl_buff_size, NULL, &cl_error);
  CHECK_ERROR(cl_error,"clCreateBuffer");

  clEnqueueWriteBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matA, 0, NULL, &ev_writeA);

  clSetKernelArg(ker, 0, sizeof(bufA), &bufA);

  size_t globalDim[] = {matrix_size, matrix_size};
  size_t localDim[] = {LOCAL_DIM_KERNEL, LOCAL_DIM_KERNEL};
   
  // GPU Calcul

    clEnqueueNDRangeKernel(command_queue, ker, 2, NULL, globalDim, localDim, 1, &ev_writeA, &ev_ker);
  clEnqueueReadBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matB, 1, &ev_ker, &ev_readA);

  clFinish(command_queue);




  // clGetEvenProfilingInfo
  
  //  clReleaseMemObject(bufA);


  /********************/
  /*** CHECK RESULT ***/   
  /********************/
  
  //printf("\nMatrix B:\n");
  //DisplayMatrix(matB,matrix_size);
  
  ClearUpMatrix(matB,matrix_size);
  printf("\nMatrix B:\n");
  DisplayMatrix(matB,matrix_size);


  TransposeMatrix(matB,matC,matrix_size);
  //printf("\nMatrix C:\n");
  //DisplayMatrix(matC,matrix_size);
  
  MullMatrix(matB,matC,matD,matrix_size);
  MinusMatrix(matD,matA,matrix_size);
  printf("\nMatrix D - A:\n");
  DisplayMatrix(matD,matrix_size);
  //  printf("\nMatrix A:\n");
  //DisplayMatrix(matA,matrix_size);
  //*/
  //  Compute with CPU

  // Compare Matrix

  free(matA);
  free(matB);
  free(matC);
  free(matD);
  
  return 0;
}


void DisplayMatrix(MATRIX_TYPE* mat, unsigned int size){
  unsigned int i,j;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      printf("%lf|"/*(%d|%d)\t"*/,mat[i+j*size]/*,i,j*/);
    }
    printf("\n");
  }
}


void InitMatrix(MATRIX_TYPE * matA, unsigned int matrix_size) {
  unsigned int i;
  for (i=0 ; i<matrix_size*matrix_size ; i++){
    matA[i] = i+1;
  }
}

void InitMatrix2(MATRIX_TYPE * matA, unsigned int matrix_size) {
  int i=0;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=5;
  matA[i++]=5;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=14;
  matA[i++]=14;
  matA[i++]=1;
  matA[i++]=5;
  matA[i++]=14;
  matA[i++]=15;
}

void ClearUpMatrix(MATRIX_TYPE * a, unsigned int size) {
  unsigned int i,j;
   for (j=0 ; j<size ; j++){
     for (i=0 ; i<size ; i++){
       if (i>j)
	 a[i+j*size] = 0;
     }
  }
}

 void TransposeMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size) {
  unsigned int i,j;
   for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      b[i+j*size] = a[j + i*size];
    }
  }
}

void MullMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, MATRIX_TYPE * c, unsigned int size) {
  unsigned int i,j,k;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      c[i+j*size]=0;
      for (k=0 ; k<size ; k++) {
	c[i+j*size] += a[k + j*size]*b[i + k*size];
      }
    }
  }
} 

void MinusMatrix(MATRIX_TYPE * a, MATRIX_TYPE * b, unsigned int size){
  unsigned int i,j;
  for (j=0 ; j<size ; j++){
    for (i=0 ; i<size ; i++){
      a[i+j*size] -= b[i+j*size];
    }
  }
}
