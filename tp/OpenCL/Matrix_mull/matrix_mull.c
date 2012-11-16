#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#include "toolsMatrix.h"
#include <sys/time.h>
#include <time.h>


int main() {

   struct timeval tv;
   double time_start;
   double time_end;
   
   srand(time(NULL));


   #define N 256
   size_t size = N * N * sizeof(float);

   printf("Matrix size : %d (%d length)\n",N,N*N);


   cl_event ev_writeA, ev_writeB, ev_ker, ev_readC;

   float * matA = malloc(N * N * sizeof(float));
   float * matB = malloc(N * N * sizeof(float));
   float * matC = malloc(N * N * sizeof(float));
   float * matD = malloc(N * N * sizeof(float));
   
   gettimeofday(&tv, NULL);  
   time_start=tv.tv_sec+(tv.tv_usec/1000000.0);  

   
   Matrix_initRand(matA,N,0,2);
   Matrix_initRand(matB,N,0,2);
   //Matrix_initNull(matA,N);
   //Matrix_initNull(matB,N);
   

   gettimeofday(&tv, NULL);  
   time_end=tv.tv_sec+(tv.tv_usec/1000000.0);

   printf("%.6lf seconds elapsed for Generate matrix\n", time_end-time_start);  

   Matrix_display(matA,N,10);
   Matrix_display(matB,N,10);

   
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
   cl_command_queue cq = clCreateCommandQueue(ctx, devs[0], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, NULL);

   // add
   /*
     FILE * f = fopen("./matrix_add.cl", "r");
     fseek(f, 0, SEEK_END);
     size_t source_size = ftell(f);
     fseek(f, 0, SEEK_SET);
   */

   // mull
   FILE * f = fopen("./matrix_mull.cl", "r");
   fseek(f, 0, SEEK_END);
   size_t source_size = ftell(f);
   fseek(f, 0, SEEK_SET);


   char * source = malloc(source_size+1);
   fread(source, 1, source_size, f);
   source[source_size] = '\0';
   fclose(f);


   //printf("Code kernel: %s\n", source);

   cl_program prg = clCreateProgramWithSource(ctx, 1, (const char**)&source, NULL, NULL);
   clBuildProgram(prg, nb_devs, devs, NULL, NULL, NULL);

   size_t log_size;
   clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
   char log[log_size];
   clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
 
   printf("Build log: %s\n", log);

   // Compilation du kernel
   cl_kernel ker = clCreateKernel(prg, "matrix_mull", NULL);

   // Création des buffers
   cl_mem bufA = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, NULL);
   cl_mem bufB = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, NULL);
   cl_mem bufC = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, NULL, NULL);

   clEnqueueWriteBuffer(cq, bufA, 0, 0, size, matA, 0, NULL, &ev_writeA);
   clEnqueueWriteBuffer(cq, bufB, 0, 0, size, matB, 0, NULL, &ev_writeB);

   clSetKernelArg(ker, 0, sizeof(bufA), &bufA);
   clSetKernelArg(ker, 1, sizeof(bufB), &bufB);
   clSetKernelArg(ker, 2, sizeof(bufC), &bufC);

   size_t globalDim[] = {N, N};
   // size_t localDim[] = {256, 1}; // pour l'add
   size_t localDim[] = {32, 32};
   cl_event deps[] = {ev_writeA, ev_writeB};
   
   // GPU Calcul
   gettimeofday(&tv, NULL);  
   time_start=tv.tv_sec+(tv.tv_usec/1000000.0);  

   clEnqueueNDRangeKernel(cq, ker, 2, NULL, globalDim, localDim, 2, deps, &ev_ker);
   clEnqueueReadBuffer(cq, bufC, 0, 0, size, matC, 1, &ev_ker, &ev_readC);
   clFinish(cq);
   // clGetEvenProfilingInfo
  
   gettimeofday(&tv, NULL);  
   time_end=tv.tv_sec+(tv.tv_usec/1000000.0);
   printf("%.6lf seconds elapsed for GPU matrix_mull\n", time_end-time_start);  

   /********************/
   /*** CHECK RESULT ***/   
   /********************/

   //  Compute with CPU
   gettimeofday(&tv, NULL);  
   time_start=tv.tv_sec+(tv.tv_usec/1000000.0);  

   Matrix_mull(matA,matB,matD,N);
   //Matrix_mull(matB,matA,matD,N);

   gettimeofday(&tv, NULL);  
   time_end=tv.tv_sec+(tv.tv_usec/1000000.0);

   printf("%.6lf seconds elapsed for CPU matrix_mull\n", time_end-time_start);  


   // Compare Matrix
   Matrix_equivalant(matC,matD,N);
   Matrix_display(matC,N,10);
   Matrix_display(matD,N,10);

   //displayMatrix(matC,N);
   //displayMatrix(matD,N);

   return 0;
}

