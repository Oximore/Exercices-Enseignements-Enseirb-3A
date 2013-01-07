#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void StartBcast(MPI_Comm comm, int* start, int* n, double* eps, int* tour_max){
  MPI_Bcast(start,1,MPI_INT,0,comm);
  MPI_Bcast(n,1,MPI_INT,0,comm);
  MPI_Bcast(eps,1,MPI_DOUBLE,0,comm);
  MPI_Bcast(tour_max,1,MPI_INT,0,comm);
}


int main (int argc, char* argv[]) {

  int myrank, mpi_size;
  int result;
  MPI_Status status;
    
  MPI_Init( NULL, NULL );
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  int matrix_size, tour_max;
  double eps;
  int i,j,start;


  MPI_Comm myComm;
  int my_row;
  int x,y;

  int work_size, work_rank;
  
  int h,hCarre;
  double norme, norme_tmp;

  int stop, nb_tour;
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
      if (matrix_size<2) {
	printf("n<2\n");
	start = 0;
      }
      tour_max = 100; 
  
      printf("n=%d eps=%lf tour_max=%d\n",matrix_size,eps,tour_max);
    }
  }
  
  StartBcast(MPI_COMM_WORLD,&start,&matrix_size,&eps,&tour_max);

  if (!start) {
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  
  // on alloue la taille de notre matrice incterne
  x = matrix_size;
  
  y = matrix_size / mpi_size;
  if (y*mpi_size < matrix_size)
    y++;
  
  if ((myrank+1)*y >= matrix_size)
    y = 0;
  
  my_row = (y==0)?0:1;
  printf("my_row=%d || y=%d\n",my_row,y);

  MPI_Comm_split(MPI_COMM_WORLD,my_row,0,&myComm); 
  
  if (!my_row) {
    MPI_Comm_size(myComm, &work_size);
    MPI_Comm_rank(myComm, &work_rank);  

    printf("my_row=%d || work_rank=%d || work_size=%d\n", my_row, work_rank, work_size);

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
	} 
      } 
      
      // Communicate
      
      if (work_rank == 0 ){
	MPI_Send(&v[x*(y-2)+2],x-2,MPI_DOUBLE,work_rank+1,98,myComm);
	
	MPI_Recv(&v[x*(y-1)+2],x+2,MPI_DOUBLE,work_rank+1,99,myComm,&status);
      }
      else if (work_rank == work_size-1) {
	MPI_Recv(&v[x+2],x+2,MPI_DOUBLE,work_rank-1,98,myComm,&status);
	
	MPI_Send(&v[2],x-2,MPI_DOUBLE,work_rank-1,99,myComm);
      }
      else {
	MPI_Recv(&v[x+2],x+2,MPI_DOUBLE,work_rank-1,98,myComm,&status);
	MPI_Send(&v[x*(y-2)+2],x-2,MPI_DOUBLE,work_rank+1,98,myComm);

	MPI_Recv(&v[x*(y-1)+2],x+2,MPI_DOUBLE,work_rank+1,99,myComm,&status);
	MPI_Send(&v[2],x-2,MPI_DOUBLE,work_rank-1,99,myComm);
      }
      
      // Continue ??
      stop = norme < eps || nb_tour > tour_max;
      MPI_Allreduce(&stop,&stop,1,MPI_INT,MPI_LAND,myComm);
    
      // on échange u et v
      tmp_u = u;
      u = v;
      v = tmp_u;
    
    }
    if (!work_rank) {
      printf("Finish with norme = %lf\n",norme);
    }
  } 
  MPI_Finalize();
  return EXIT_SUCCESS;
}
