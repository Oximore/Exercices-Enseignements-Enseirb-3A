#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "cholesky.h"
#include "util.h"

/*** Prototypes ***/
void CreateKernel(cl_kernel* kernel, cl_context ctx,   cl_device_id* devs, cl_uint nb_devs, const char* file_name, const char* function_name,bool verbose);


/*** Functions ***/

int main(int argc, char* argv[]) {
  cl_int cl_error;

  /** Init **/
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
  
  printf("Nombre de plateformes: %d\t", nb_platf);
  
  cl_platform_id platfs[nb_platf];
  clGetPlatformIDs(nb_platf, platfs, NULL);
  
  size_t plat_name_size;
  clGetPlatformInfo(platfs[0], CL_PLATFORM_NAME, 0, NULL, &plat_name_size);
  char plat_name[plat_name_size];
  clGetPlatformInfo(platfs[0], CL_PLATFORM_NAME, plat_name_size, &plat_name, NULL);
  printf("( %s ", plat_name);

  size_t plat_vendor_size;
  clGetPlatformInfo(platfs[0], CL_PLATFORM_VENDOR, 0, NULL, &plat_vendor_size);
  char plat_vendor[plat_vendor_size];
  clGetPlatformInfo(platfs[0], CL_PLATFORM_VENDOR, plat_vendor_size, &plat_vendor, NULL);
  printf("| %s)\n", plat_vendor);

  cl_uint nb_devs;
  clGetDeviceIDs(platfs[0], CL_DEVICE_TYPE_ALL, 0, NULL, &nb_devs);

  cl_device_id devs[nb_devs];
  clGetDeviceIDs(platfs[0], CL_DEVICE_TYPE_ALL, nb_devs, devs, NULL);

  
  cl_context ctx = clCreateContext(NULL, nb_devs, devs, NULL, NULL, NULL);
  cl_command_queue command_queue = clCreateCommandQueue(ctx, devs[0], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, NULL);


  /** Chargement des kernels **/
  cl_kernel ker1, ker2, ker3;

  bool verbose = false;
  CreateKernel(&ker1, ctx, devs, nb_devs, "./cholesky_kernel1.cl", "cholesky_diag", verbose);
  CreateKernel(&ker2, ctx, devs, nb_devs, "./cholesky_kernel2.cl", "cholesky_inf", verbose);
  CreateKernel(&ker3, ctx, devs, nb_devs, "./cholesky_kernel3.cl", "cholesky_subdiag", verbose);

  int nombre_de_kernel = 3;
  cl_event ev_ker[nombre_de_kernel], ev_readA;

  // Création/Préparation du buffer GPU
  cl_mem bufA = clCreateBuffer(ctx, CL_MEM_READ_WRITE, cl_buff_size, NULL, &cl_error);
  CHECK_ERROR(cl_error,"clCreateBuffer");

  clEnqueueWriteBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matA, 0, NULL, &ev_ker[nombre_de_kernel-1]);
  // ev_ker[n-1] pour préparer la boucle.

  size_t globalDim[] = {matrix_size, matrix_size};
  size_t localDim[] = {LOCAL_DIM_KERNEL, LOCAL_DIM_KERNEL};
   

  // GPU Calculs
  int i;
  for (i=0 ; i<matrix_size/LOCAL_DIM_KERNEL/**/ ; i++) {
    // Kernel 1 : Bloc diagonal
    clSetKernelArg(ker1, 0, sizeof(bufA), &bufA);
    clSetKernelArg(ker1, 1, sizeof(i), &i);
    clEnqueueNDRangeKernel(command_queue, ker1, 2, NULL, globalDim, localDim, 1, &ev_ker[nombre_de_kernel-1], &ev_ker[0]);
    
    // Kernel 2 : Blocs sous-diagonaux inferieur 
    clSetKernelArg(ker2, 0, sizeof(bufA), &bufA);
    clSetKernelArg(ker2, 1, sizeof(i), &i);
    clEnqueueNDRangeKernel(command_queue, ker2, 2, NULL, globalDim, localDim, 1, &ev_ker[0], &ev_ker[1]);
    

    // Kernel 3 : Blocs sous-diagonaux
    clSetKernelArg(ker3, 0, sizeof(bufA), &bufA);
    clSetKernelArg(ker3, 1, sizeof(i), &i);
    clEnqueueNDRangeKernel(command_queue, ker3, 2, NULL, globalDim, localDim, 1, &ev_ker[1], &ev_ker[2]);
  }
      
  clEnqueueReadBuffer(command_queue, bufA, CL_TRUE, 0, cl_buff_size, matB, 1, &ev_ker[nombre_de_kernel-1], &ev_readA);
  
  clFinish(command_queue);

  // clGetEvenProfilingInfo
  clReleaseMemObject(bufA);


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
