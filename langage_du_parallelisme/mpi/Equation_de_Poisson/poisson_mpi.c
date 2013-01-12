// By Blux & Pyc
// Equation de possoin (chaleur), modélisation et encodage à l'aide de mpi


/*** Includes ***/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "test_code.h"

/*** Defines ***/
#define DOWN  0
#define UP    1
#define RIGHT 2
#define LEFT  3

#define WORK_TAG 99


#define DEBUG(a)          printf("rank:%d, %s\n",myrank ,a)
#define NULLTEST(a)       (a==NULL) ? "*":"!"
#define CHECKSTATUS(s) 	  //if (s != MPI_SUCCESS) printf("rank%d : ERROR WITH STATUS\n",grid_rank)
#define INIT_VALUE 1.0

/*** Prototypes ***/
void printMatrice(double** u, int n, int w, int h);
double d_abs(double a);
void InitBcast(MPI_Comm comm, int* size_matrix, double* eps, int* max_round, int* horizontal_processor_number, int* vertical_processor_number);


/*** Functions ***/
void printMatrice(double** u, int n, int w, int h){
  int i,j;
  for (j=0;j<h;j++){
    for (i=0;i<w;i++){
      printf("%f\t",u[n][i+j*w]);
    }
    printf("\n");
  }
}


double d_abs(double a){
  return (a<0)? -a:a;
}


void InitBcast(MPI_Comm comm, int* size_matrix, double* eps, int* max_round, int* horizontal_processor_number, int* vertical_processor_number){
  MPI_Bcast(size_matrix,1,MPI_INT,0,comm);
  MPI_Bcast(eps,1,MPI_DOUBLE,0,comm);
  MPI_Bcast(max_round,1,MPI_INT,0,comm);
  MPI_Bcast(horizontal_processor_number,1,MPI_INT,0,comm);
  MPI_Bcast(vertical_processor_number,1,MPI_INT,0,comm);
}



int main (int argc, char* argv[]) {

  int myrank, mpi_size;
    
  MPI_Init( NULL, NULL );
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  int matrix_size, max_round;
  double eps;
  int i,j,start;
  int horizontal_processor_number, vertical_processor_number;
  int height_local, weight_local;

  MPI_Comm workComm;
  int my_row;

  double h,hCarre;
  double norme, norme_tmp;

  int current_indice, other_indice;
  int stop, stop_tmp, nb_tour;
  int x, y, indice;
  int y_min, y_max;
  int x_min, x_max;    
  
  double **u_double, *u, *v;

  /*** INIT ***/
  
  /* Arg filter */
  start = 0;
  if (myrank==0) {
    if (argc !=6) {
      printf("usage : %s eps size_matrix max_round P Q\n",argv[0]);
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

  // Brodcast information about argument to all the process
  MPI_Bcast(&start,1,MPI_INT,0,MPI_COMM_WORLD);
  if (start==0) {
    printf("Error - Program close\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
  
  InitBcast(MPI_COMM_WORLD,&matrix_size,&eps,&max_round,&horizontal_processor_number,&vertical_processor_number);
  
  // Split the needed process
  // Si je dois travailler, my_row = 0, sinon my_row = 1
  my_row = (myrank < vertical_processor_number*horizontal_processor_number)?0:1;
  MPI_Comm_split(MPI_COMM_WORLD,my_row,0,&workComm); 
  
  if (my_row == 0) {
    int grid_rank, my_coords[2];
    int position[4];
    
    int dims[2] =  {horizontal_processor_number,vertical_processor_number};
    int periods[2] = {0,0};
    int reorder = 1;
    
    MPI_Comm grid_2D;  
    MPI_Cart_create(workComm, 2, dims, periods, reorder, &grid_2D);

    MPI_Comm_rank(grid_2D, &grid_rank);  
    MPI_Cart_coords(grid_2D, grid_rank, 2, my_coords);
    
    // Description of sub matrix position into the big matrix
    // Bool vector
    position[LEFT]  = (my_coords[0] == 0) ? 1:0;
    position[RIGHT] = (my_coords[0] == horizontal_processor_number-1) ? 1:0;
    position[UP]    = (my_coords[1] == 0) ? 1:0;
    position[DOWN]  = (my_coords[1] == vertical_processor_number-1) ? 1:0;
          
    // Calcul de la taille de la matrice
    weight_local = matrix_size/horizontal_processor_number;
    height_local = matrix_size/vertical_processor_number;
    
    if (position[LEFT] && matrix_size%horizontal_processor_number != 0 ) {
      weight_local = matrix_size % horizontal_processor_number;
    }
    if (position[DOWN]
	&& matrix_size%vertical_processor_number != 0) { 
      height_local = matrix_size % vertical_processor_number; 
    }

    if (horizontal_processor_number > 1)
      weight_local++;
    if (!position[LEFT] && !position[RIGHT])
      weight_local++;
    
    if(vertical_processor_number > 1)
      height_local++;
    if (!position[UP] && !position[DOWN])
      height_local++;
    
    //  double **u_double, *u, *v;
    // Alloc of my part of the matrix
    u_double    = malloc(sizeof(*u_double)*2);    
    u_double[0] = malloc(sizeof(**u_double)*weight_local*height_local); // free ?
    u_double[1] = malloc(sizeof(**u_double)*weight_local*height_local); // free ?
    

    for ( i=0 ; i<weight_local*height_local ; i++) {
      u_double[0][i] = INIT_VALUE;
      u_double[1][i] = INIT_VALUE;
    }
    for ( i=0 ; i<weight_local ; i++) {
      u_double[0][i] = 0.0;
      u_double[1][i] = 0.0;

      u_double[0][(height_local-1)*weight_local+i] = 0.0;
      u_double[1][(height_local-1)*weight_local+i] = 0.0;
    }
    for ( i=0 ; i<height_local ; i++) {
      u_double[0][i*weight_local] = 0.0;
      u_double[1][i*weight_local] = 0.0;

      u_double[0][(i+1)*weight_local-1] = 0.0;
      u_double[1][(i+1)*weight_local-1] = 0.0;
    }

 
    //printf("myrank:%d || my_row:%d || grid_rank:%d || my_coords:%d;%d || sixe:w:%d/h:%d \n", myrank, my_row, grid_rank, my_coords[0], my_coords[1], weight_local, height_local);


    // Init Communication
    MPI_Request sendRequests[2][4];
    MPI_Request recvRequests[2][4];
    MPI_Status  statusRequests[4];

    int otherCoord[2];    
    int otherRank;
    
    // création du type
    MPI_Datatype column_type;
    MPI_Type_vector(height_local, 1, weight_local, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);
    
    for ( i=0 ; i<2 ; i++) {
      // Horizontals communications
      if (horizontal_processor_number == 1) {
	sendRequests[i][LEFT]   = NULL;
	recvRequests[i][LEFT]   = NULL;	
	
	sendRequests[i][RIGHT] = NULL;
	recvRequests[i][RIGHT] = NULL;
      }
      else if (position[LEFT]) {
	// with right
	otherCoord[0] = my_coords[0]+1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][2*weight_local-2], 1, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][RIGHT]);
	MPI_Recv_init(&u_double[i][2*weight_local-1], 1, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][RIGHT]);

	sendRequests[i][LEFT]   = NULL;
	recvRequests[i][LEFT]   = NULL;	
      }
      else if (position[RIGHT]) {
	// with left
	otherCoord[0] = my_coords[0]-1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], 1, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][LEFT]);
	MPI_Recv_init(&u_double[i][weight_local],   1, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][LEFT]);

	sendRequests[i][RIGHT] = NULL;
	recvRequests[i][RIGHT] = NULL;
      }
      else {
	// with left
	otherCoord[0] = my_coords[0]-1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], 1, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][LEFT]);
	MPI_Recv_init(&u_double[i][weight_local],   1, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][LEFT]);
	
	// and with right
	otherCoord[0] = my_coords[0]+1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][2*weight_local-2], 1, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][RIGHT]);
	MPI_Recv_init(&u_double[i][2*weight_local-1], 1, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][RIGHT]);
	
      }
      
      // Verticals communications
      if (vertical_processor_number == 1) {
	sendRequests[i][UP]   = NULL;
	recvRequests[i][UP]   = NULL;	

	sendRequests[i][DOWN] = NULL;
	recvRequests[i][DOWN] = NULL;
      }
      else if (position[UP]) {
	// with down
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]+1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][DOWN]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][DOWN]);
	
	sendRequests[i][UP]   = NULL;
	recvRequests[i][UP]   = NULL;	
      }
      else if (position[DOWN]) {
	// with up
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]-1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][UP]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][UP]);

	sendRequests[i][DOWN] = NULL;
	recvRequests[i][DOWN] = NULL;
      }
      else {
	// with up
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]-1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][UP]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][UP]);

	// and with down
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]+1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][DOWN]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][DOWN]);
      }
    }

    // Init compute 
    h = 1.0/(matrix_size-1);
    hCarre = h*h;
    //printf("h²:%lf || h:%lf\n",hCarre,h);
    norme = eps+1;
    stop = 0;
    nb_tour = 0;
    
    y_min=1; y_max=height_local-1;
    x_min=1; x_max=weight_local-1;    
    
    u = u_double[0];
    v = u_double[1];

    int MatrixAPrint = -1; /* TODO a suppr */
    
    // Debug Print matrix
    MPI_Barrier(grid_2D);
    if (grid_rank==MatrixAPrint){
      printMatrice(u_double, 0, weight_local, height_local);
      printf("\n");
      printMatrice(u_double, 1, weight_local, height_local);
      printf("\n");
    }
    MPI_Barrier(grid_2D);
    //*/
    
    while (!stop) {
      //printf("%d: round %d\n", grid_rank, nb_tour);
      current_indice = nb_tour%2;
      other_indice   = (nb_tour+1)%2;
      u = u_double[current_indice];
      v = u_double[other_indice];
      
      norme = 0;
      // Compute border      
      // a. horizontal border
      for (y=0 ; y<2 ; y++) {
	for (i=1; i<weight_local-1 ; i++) {
	  j = 1 + (height_local-3)*y;
	  
	  indice = i + weight_local*j;
	  v[indice] = 0.25*(u[indice-weight_local] + u[indice+weight_local] + u[indice-1] + u[indice+1] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = d_abs(v[indice] - u[indice]);
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	}
      }
      // b. vertical border
      for (x=0 ; x<2 ; x++) {
	for (j=1; j<weight_local ; j++) {
	  i = 1 + (weight_local-3)*x;
	  
	  indice = i + weight_local*j;
	  v[indice] = 0.25*(u[indice-weight_local] + u[indice+weight_local] + u[indice-1] + u[indice+1] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = d_abs(v[indice] - u[indice]);
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	}
      }
      // End borders compute


      // Communicate
      for (i=0 ; i<4 ; i++) {
	if (sendRequests[other_indice][i] != NULL)
	  MPI_Start(&sendRequests[other_indice][i]);
	if (recvRequests[other_indice][i] != NULL)
	  MPI_Start(&recvRequests[other_indice][i]);
      }
      

      // Compute interior
      for (j=y_min+1 ; j<y_max ; ++j){
	for (i=x_min+1 ; i<x_max ; ++i){
	  indice = i + weight_local*j;
	  v[indice] = 0.25*(u[indice-weight_local] + u[indice+weight_local] + u[indice-1] + u[indice+1] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = d_abs(v[indice] - u[indice]);
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	  //	  printf("*** tour:%d || work_rank:%d || norme_tmp:%lf\n",nb_tour,work_rank,norme_tmp);
	} 
      } 

      // Wait end of comm
      //MPI_Waitall(4,sendRequests[other_indice],statusRequests);
      //MPI_Waitall(4,recvRequests[other_indice],statusRequests);
            
      for (i=0 ; i<4 ; i++){
	if(sendRequests[other_indice][i] != NULL) {	
	  MPI_Wait(&sendRequests[other_indice][i], statusRequests);	
	  CHECKSTATUS(statusRequests);
	}
	if(recvRequests[other_indice][i] != NULL){	
	  MPI_Wait(&recvRequests[other_indice][i], statusRequests);	
	  CHECKSTATUS(statusRequests);
	}
      }
      
      // Continue ??
      stop = norme < eps || nb_tour > max_round;
      //printf("*** tour:%d || grid_rank:%d || norme:%lf || stop=%d?\n",nb_tour,grid_rank,norme,stop);
      MPI_Allreduce(&stop,&stop_tmp,1,MPI_INT,MPI_LAND,grid_2D);
      stop = stop_tmp;

      /*
      if (!grid_rank) {
	printf("Tour:%d, finish with norme = %lf (rank=%d)\n", nb_tour, norme, grid_rank);
      }
      //*/
      
      /*     
      // Debug Print matrix
      MPI_Barrier(grid_2D);
      if (grid_rank==MatrixAPrint){
	printMatrice(u_double, 0, weight_local, height_local);
	printf("\n");
	printMatrice(u_double, 1, weight_local, height_local);
	printf("\n");
      }
      MPI_Barrier(grid_2D);
      //*/

      nb_tour++;
      } /* end while(!stop) */

    
    // Debug Print matrix
    MPI_Barrier(grid_2D);
    if (grid_rank==MatrixAPrint){
      printMatrice(u_double, 0, weight_local, height_local);
      printf("\n");
      printMatrice(u_double, 1, weight_local, height_local);
      printf("\n");
    }
    MPI_Barrier(grid_2D);
    //*/

    //DEBUG("I have finish my work");

    // Check results
    double finalDiff = -1, tmpDiff; 
    x = my_coords[0]*(matrix_size/horizontal_processor_number) ;
    y = my_coords[1]*(matrix_size/vertical_processor_number) ; 
    //printf("(%d,%d)  ||  height_local:%d || weight_local:%d\n", my_coords[0],my_coords[1],height_local,weight_local);
    for (j=1 ; j<height_local-2 ; j++) {
      for (i=1 ; i<weight_local-2 ; i++) {
	tmpDiff = d_abs(v[i + j*weight_local] - solution(h*(x+i),h*(y+j)));
	if (tmpDiff > finalDiff)
	  finalDiff = tmpDiff;
      }
    }
    //printf("%d says: tmp Diff %lf\n",grid_rank,finalDiff );

    tmpDiff = finalDiff;
    MPI_Allreduce(&tmpDiff,&finalDiff,1,MPI_DOUBLE,MPI_MAX,grid_2D);
    
    if (grid_rank==0) {
      printf("Final Diff : %lf (%d itérations)\n",finalDiff, nb_tour);
    }
    

  } /* end if(my_row==0) */ 
  MPI_Finalize();
  return EXIT_SUCCESS;
}
