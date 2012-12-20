#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include <unistd.h>



int main(int argc, char* argv[]) {
  
  cl_event ev_writeA, ev_ker, ev_readB;
  
#define N 32

  float * matA = malloc(N * N * sizeof(float));
  float * matB = malloc(N * N * sizeof(float));

  int i,j;
  for (i = 0 ; i< N*N ; i++){
    matA[0] = matB[0] = 0;
  }
  
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


  // mull
  FILE * f = fopen("./file_test2.cl", "r");
  fseek(f, 0, SEEK_END);
  size_t source_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char * source = malloc(source_size+1);
  fread(source, 1, source_size, f);
  source[source_size] = '\0';
  fclose(f);

  printf("Code kernel: %s\n", source);

  cl_program prg = clCreateProgramWithSource(ctx, 1, (const char**)&source, NULL, NULL);
  char buildOption[512] = "-I ";

  char* current_directory = get_current_dir_name();

  strcat(buildOption,current_directory);
  strcat(buildOption,"/");
  free(current_directory);
  // printf("BUILD OPTION : %s\n\n",buildOption);
  
  clBuildProgram(prg, nb_devs, devs, buildOption, NULL, NULL);

  size_t log_size;
  clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
  char log[log_size];
  clGetProgramBuildInfo(prg, devs[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
 
  printf("Build log: %s\n", log);

  // Compilation du kernel
  cl_kernel kerTest = clCreateKernel(prg, "test", NULL);

  // Création des buffers
  int size = N*N*sizeof(float);
  cl_mem bufA = clCreateBuffer(ctx, CL_MEM_READ_WRITE, size, NULL, NULL);
  clEnqueueWriteBuffer(cq, bufA, 0, 0, size, matA, 0, NULL, &ev_writeA);

  clSetKernelArg(kerTest, 0, sizeof(bufA), &bufA);

  size_t globalDim[] = {N, N};
  size_t localDim[] = {8, 8};
  
  printf("Calculs GPU\n");
  // GPU Calcul
  clEnqueueNDRangeKernel(cq, kerTest, 2, NULL, globalDim, localDim, 1, &ev_writeA, &ev_ker);
  clFinish(cq);
  printf("Calculs GPU mi-finis\n");
  clEnqueueReadBuffer(cq, bufA, CL_TRUE, 0, size, matB, 1, &ev_ker, &ev_readB);
  clFinish(cq);
  printf("Calculs GPU finis\n");

  // clGetEvenProfilingInfo
  
  /********************/
  /*** CHECK RESULT ***/   
  /********************/
  

  //  Compute with CPU
  
  for (j=0 ; j<N ; j++){
    for (i=0 ; i<N ; i++) {
      printf("%f(%d|%d)\t",matB[i+N*j],i,j);
    }
    printf("\n");
  }
  return 0;
}

