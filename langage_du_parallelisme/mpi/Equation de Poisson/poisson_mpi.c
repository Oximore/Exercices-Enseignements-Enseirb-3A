#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "test_code.h"


void StartBcast(MPI_Comm comm, int* start, int* size_matrix, double* eps, int* max_round, int* horizontal_processor_number, int* vertical_processor_number){
  MPI_Bcast(start,1,MPI_INT,0,comm);
  if (!start) {
    printf("Error - Program close\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  
  MPI_Bcast(size_matrix,1,MPI_INT,0,comm);
  MPI_Bcast(eps,1,MPI_DOUBLE,0,comm);
  MPI_Bcast(max_round,1,MPI_INT,0,comm);
  MPI_Bcast(horizontal_processor_number,1,MPI_INT,0,comm);
  MPI_Bcast(vertical_processor_number,1,MPI_INT,0,comm);
}


int main (int argc, char* argv[]) {

  int myrank, mpi_size;
  MPI_Status status;
    
  MPI_Init( NULL, NULL );
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  int matrix_size, max_round;
  double eps;
  int i,j,start;
  int horizontal_processor_number, vertical_processor_number;
  int height_local, weight_local;
  int x_gloabl, y_global;  

  MPI_Comm workComm;
  int my_row;

  int work_size, work_rank;
  
  double h,hCarre;
  double norme, norme_tmp;

  int stop, stop_tmp, nb_tour;
  int x,y;
  int y_min, y_max;
  int x_min, x_max;    
  
  double *tmp_u, *u, *v;

  /*** INIT ***/


  /* Arg filter */
  start = 0;
  if (!myrank) {
    if (argc !=3) {
      printf("usage : %s eps size_matrix\n",argv[0]);
      start = 0;
    }
    else {
      start = 1;
      
      i = 0;
      sscanf(argv[++i],"%lf",&eps);
      matrix_size = atoi(argv[++i]);
      max_round = atoi(argv[++i]); 
      horizontal_processor_number = atoi(argv[++i]);
      vertical_processor_number = atoi(argv[++i]);

      if (matrix_size<2) {
	printf("n<2\n");
	start = 0;
      }
      if (horizontal_processor_number*vertical_processor_number>mpi_size){
	printf("not engouth processor.\n");
	start = 0;
      }
      
      printf("n=%d eps=%lf max_round=%d hori_pnb=%d vert_pnb=%d\n",matrix_size,eps,max_round,horizontal_processor_number,vertical_processor_number);
    }
  }
  
  StartBcast(MPI_COMM_WORLD,&start,&matrix_size,&eps,&max_round,&horizontal_processor_number,&vertical_processor_number);
  
  // Si je dois travailler, my_row = 0, sinon my_row = 1
  my_row = (myrank < vertical_processor_number*horizontal_processor_number)?0:1;
  MPI_Comm_split(MPI_COMM_WORLD,my_row,0,&workComm); 
  
  if (my_row == 0) {
    int grid_ran, coords;

    
    int dims[2] =  {vertical_processor_number,horizontal_processor_number};
    int periods[2] = {0,0};
    int reorder = 1;
    
    
    MPI_Comm grid_2D;  
    MPI_Cart_create(workComm, 2, dims, periods, reorder, &grid_2D);

    //MPI_Comm_size(grid_2D, &work_size);
        

    MPI_Cart_rank(grid_2D, &grid_rank);  
    MPI_Cart_coords(grid_2D,grid_rank, 1, &coords);
    y_gloabl = coords;

    MPI_Cart_coords(grid_2D, grid_rank, 2, &coords);
    x_gloabl = coords;
    

    //    printf("my_row=%d || work_rank=%d || myrank=%d || work_size=%d\n", my_row, work_rank, myrank, work_size);




  // on alloue la taille de notre matrice interne
  
  x_gloabl = (myrank) % horizontal_processor_number;
  y_global = (myrank) / horizontal_processor_number;  

  // séparer directement si je suis trop grand

  weight_local = matrix_size/horizontal_processor_number;
  height_local = matrix_size/vertical_processor_number;
  
  if (x_gloabl == horizontal_processor_number-1  // Si je suis sur le coté gauche
      && matrix_size%horizontal_processor_number != 0 ) { // Et que le coté gauche doit être rogné
    weight_local = matrix_size % horizontal_processor_number;
  }
  if (y_global == vertical_processor_number-1  // si je suis en bas
      && matrix_size%vertical_processor_number != 0) { 
    height_local = matrix_size % vertical_processor_number; 
  }

  if (horizontal_processor_number > 1){
    weight_local++;
  }
  if (x_gloabl!=0 && x_gloabl!=horizontal_processor_number-1) {
    weight_local++;
  }
  
  if(vertical_processor_number > 1) {
    height_local++;
  }
  if (y_global!=0 && y_global!=vertical_processor_number-1) {
    height_local++;
  }











    y++;
    if (work_rank != 0 || work_rank == work_size-1) {
      y++;
    }
    
    y_min=1; y_max=y-1;
    x_min=1; x_max=x-1;    
    
    u = malloc(sizeof(*u)*x*y); // free ?
    v = malloc(sizeof(*v)*x*y); // free ?
    for ( i=0 ; i<x*y ; i++) {
      u[i] = 0.0;
      v[i] = 0.0;
    }
    
    h = 1.0/(x-1);
    hCarre = h*h;
    //printf("h²:%lf || h:%lf\n",hCarre,h);
    norme = eps+1;
    stop = 0;
    nb_tour = 0;
    
    while (!stop) {
      nb_tour++;

      // Compute
      norme = 0;
      for ( j=y_min ; j<=y_max ; ++j){
	for ( i=x_min ; i<=x_max ; ++i){
	  
	  v[i + x*j] = 0.25*(u[(i-1) + x*j] + u[(i+1) + x*j] + u[i + x*(j-1)] + u[i + x*(j+1)] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = v[i + x*j] - u[i + x*j];
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	  else if (norme < -norme_tmp )
	    norme = -norme_tmp;
	  //	  printf("*** tour:%d || work_rank:%d || norme_tmp:%lf\n",nb_tour,work_rank,norme_tmp);
	} 
      } 
      
      // Communicate
      
      if (work_rank == 0 ){
	MPI_Send(&v[x*(y-2)+2],x-2,MPI_DOUBLE,work_rank+1,98,workComm);
	
	MPI_Recv(&v[x*(y-1)+2],x+2,MPI_DOUBLE,work_rank+1,99,workComm,&status);
      }
      else if (work_rank == work_size-1) {
	MPI_Recv(&v[x+2],x+2,MPI_DOUBLE,work_rank-1,98,workComm,&status);
	
	MPI_Send(&v[2],x-2,MPI_DOUBLE,work_rank-1,99,workComm);
      }
      else {
	MPI_Recv(&v[x+2],x+2,MPI_DOUBLE,work_rank-1,98,workComm,&status);
	MPI_Send(&v[x*(y-2)+2],x-2,MPI_DOUBLE,work_rank+1,98,workComm);

	MPI_Recv(&v[x*(y-1)+2],x+2,MPI_DOUBLE,work_rank+1,99,workComm,&status);
	MPI_Send(&v[2],x-2,MPI_DOUBLE,work_rank-1,99,workComm);
      }
      
      // Continue ??
      stop = norme < eps || nb_tour > max_round;
      //printf("*** tour:%d || work_rank:%d || norme:%lf\n",nb_tour,work_rank,norme);
      MPI_Allreduce(&stop,&stop_tmp,1,MPI_INT,MPI_LAND,workComm);
      stop = stop_tmp;
      
      // on échange u et v
      tmp_u = u;
      u = v;
      v = tmp_u;
    
    }
    //*/
    if (!work_rank) {
      printf("Tour:%d, finish with norme = %lf\n",nb_tour,norme);
    }
  } 
  MPI_Finalize();
  return EXIT_SUCCESS;
}
