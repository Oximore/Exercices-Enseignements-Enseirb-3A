#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <string.h>
#include <unistd.h>

#include "cholesky_monokernel.h"
#include <sys/time.h>
#include <time.h>


int main(int argc, char* argv[]) {
  
  cl_int cl_error;
   
  unsigned int matrix_size = MATRIX_SIZE;
  unsigned int matrix_total_size = matrix_size*matrix_size;
  size_t cl_buff_size = matrix_total_size * sizeof(MATRIX_TYPE);
  
  printf("Matrix size : %d (%d length)\t |\t sous bloc de %d\n",matrix_size,matrix_total_size,LOCAL_DIM_KERNEL);
  
  MATRIX_TYPE * matA = malloc(matrix_total_size * sizeof(*matA));
  MATRIX_TYPE * matB = malloc(matrix_total_size * sizeof(*matB));
  MATRIX_TYPE * matC = malloc(matrix_total_size * sizeof(*matC));
  MATRIX_TYPE * matD = malloc(matrix_total_size * sizeof(*matD));
    
  // Init matA
  InitMatrix2(matA, matrix_size);
  //  InitMatrix(matB, matrix_size);
  
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
  
  // Allouer des buffer non contigues ?
  cl_event ev_readA;
  
  cl_kernel ker1, ker2, ker3;
  bool verbose = false;
  CreateKernel(&ker1, ctx, devs, nb_devs, "./cholesky_kernel1.cl", "cholesky_diag", verbose);
  CreateKernel(&ker2, ctx, devs, nb_devs, "./cholesky_kernel2.cl", "cholesky_inf", verbose);
  CreateKernel(&ker3, ctx, devs, nb_devs, "./cholesky_kernel3.cl", "cholesky_subdiag", verbose);

  int nombre_de_kernel = 3;
  cl_event ev_ker[nombre_de_kernel];

  // Création des buffers
  cl_mem bufA = clCreateBuffer(ctx, CL_MEM_READ_WRITE, cl_buff_size, NULL, &cl_error);
  CHECK_ERROR(cl_error,"clCreateBuffer");

  clEnqueueWriteBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matA, 0, NULL, &ev_ker[nombre_de_kernel-1]);
  // ev_ker[n-1] pour préparer la boucle.

  size_t globalDim[] = {matrix_size, matrix_size};
  size_t localDim[] = {LOCAL_DIM_KERNEL, LOCAL_DIM_KERNEL};
   
  // GPU Calcul

  int i;
  printf("MON FUC*** INDICE : %d\n",matrix_size/LOCAL_DIM_KERNEL);
  for (i=0 ; i<matrix_size/LOCAL_DIM_KERNEL/**/ ; i++) {
    // Kernel 1
    clSetKernelArg(ker1, 0, sizeof(bufA), &bufA);
    clSetKernelArg(ker1, 1, sizeof(i), &i);
    clEnqueueNDRangeKernel(command_queue, ker1, 2, NULL, globalDim, localDim, 1, &ev_ker[nombre_de_kernel-1], &ev_ker[0]);
    
    if (nombre_de_kernel>1) {
      // Kernel 2 
      clSetKernelArg(ker2, 0, sizeof(bufA), &bufA);
      clSetKernelArg(ker2, 1, sizeof(i), &i);
      clEnqueueNDRangeKernel(command_queue, ker2, 2, NULL, globalDim, localDim, 1, &ev_ker[0], &ev_ker[1]);
    }

    if (nombre_de_kernel>2){
      // Kernel 3
      clSetKernelArg(ker3, 0, sizeof(bufA), &bufA);
      clSetKernelArg(ker3, 1, sizeof(i), &i);
      clEnqueueNDRangeKernel(command_queue, ker3, 2, NULL, globalDim, localDim, 1, &ev_ker[1], &ev_ker[2]);
    }
  }

  clFinish(command_queue);
    
  clEnqueueReadBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matB, 1, &ev_ker[nombre_de_kernel-1], &ev_readA);
  
  clFinish(command_queue);

  // clGetEvenProfilingInfo
  
  //  clReleaseMemObject(bufA);


  /********************/
  /*** CHECK RESULT ***/   
  /********************/
  //  ClearUpMatrix(matB,matrix_size);
  printf("\nMatrix B:\n");
  DisplayMatrix(matB,matrix_size);
  /**/
  ClearUpMatrix(matB,matrix_size);
  TransposeMatrix(matB,matC,matrix_size);
  MullMatrix(matB,matC,matD,matrix_size);
  MinusMatrix(matD,matA,matrix_size);

  printf("\nMatrix D - A:\n");
  DisplayMatrix(matD,matrix_size);
  //*/
  // Free
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

  
void CreateKernel(cl_kernel* kernel, cl_context ctx,   cl_device_id* devs, cl_uint nb_devs, const char* file_name, const char* function_name,bool verbose) {
  cl_int cl_error;
  
  FILE * f = fopen(file_name, "r");
  fseek(f, 0, SEEK_END);
  size_t source_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char * source = malloc(source_size+1);
  fread(source, 1, source_size, f);
  source[source_size] = '\0';
  fclose(f);

  if (verbose)
    printf("Code kernel: %s\n", source);

  char buildOption[1024] = "-I ";
  char* current_directory = getcwd(NULL,0);
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
  printf("Build \"%s\" log: %s\n",file_name, log);

  // Compilation du kernel
  *kernel = clCreateKernel(prg, function_name, &cl_error);
  CHECK_ERROR(cl_error,"clCreateKernel");
  
  
}
