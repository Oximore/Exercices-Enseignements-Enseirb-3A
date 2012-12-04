#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "test_code.h"


#define DOWN  0
#define UP    1
#define RIGHT 2
#define LEFT  3

#define WORK_TAG 99

#define DEBUG(a) printf("rank:%d, %s\n",myrank ,a)


void InitBcast(MPI_Comm comm, int* size_matrix, double* eps, int* max_round, int* horizontal_processor_number, int* vertical_processor_number){
  MPI_Bcast(size_matrix,1,MPI_INT,0,comm);
  MPI_Bcast(eps,1,MPI_DOUBLE,0,comm);
  MPI_Bcast(max_round,1,MPI_INT,0,comm);
  MPI_Bcast(horizontal_processor_number,1,MPI_INT,0,comm);
  MPI_Bcast(vertical_processor_number,1,MPI_INT,0,comm);
}



int main (int argc, char* argv[]) {

  int myrank, mpi_size;
  //MPI_Status status;
    
  MPI_Init( NULL, NULL );
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  int matrix_size, max_round;
  double eps;
  int i,j,start;
  int horizontal_processor_number, vertical_processor_number;
  int height_local, weight_local;
  //int x_gloabl, y_global;  

  MPI_Comm workComm;
  int my_row;

  //  int work_rank;
  //  int work_size;

  double h,hCarre;
  double norme, norme_tmp;

  int stop, stop_tmp, nb_tour;
  int x, y, indice;
  int y_min, y_max;
  int x_min, x_max;    
  
  double **u_double, *u, *v;

  /*** INIT ***/
  DEBUG("Hi");
  
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
  
  MPI_Bcast(&start,1,MPI_INT,0,MPI_COMM_WORLD);
  if (start==0) {
    printf("Error - Program close\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
  
  InitBcast(MPI_COMM_WORLD,&matrix_size,&eps,&max_round,&horizontal_processor_number,&vertical_processor_number);
  
  // Si je dois travailler, my_row = 0, sinon my_row = 1
  my_row = (myrank < vertical_processor_number*horizontal_processor_number)?0:1;
  MPI_Comm_split(MPI_COMM_WORLD,my_row,0,&workComm); 
  
  if (my_row == 0) {

    //    DEBUG("I have to work");
    int grid_rank, my_coords[2];
    int position[4];
    
    int dims[2] =  {horizontal_processor_number,vertical_processor_number};
    int periods[2] = {0,0};
    int reorder = 1;
    
    MPI_Comm grid_2D;  
    MPI_Cart_create(workComm, 2, dims, periods, reorder, &grid_2D);

    MPI_Comm_rank(grid_2D, &grid_rank);  
    MPI_Cart_coords(grid_2D, grid_rank, 2, my_coords);
    
    // vecteur de booléens
    position[LEFT]  = (my_coords[0] == 0) ? 1:0;
    position[RIGHT] = (my_coords[0] == horizontal_processor_number-1) ? 1:0;
    position[UP]    = (my_coords[1] == 0) ? 1:0;
    position[DOWN]  = (my_coords[1] == vertical_processor_number-1) ? 1:0;
          
    // Calcul de la taille de la matrice
    weight_local = matrix_size/horizontal_processor_number;
    height_local = matrix_size/vertical_processor_number;
    
    if (position[LEFT] && matrix_size%horizontal_processor_number != 0 ) { // Et que le coté gauche doit être rogné
      weight_local = matrix_size % horizontal_processor_number;
    }
    if (position[DOWN]  // si je suis en bas
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
    
    u_double = malloc(sizeof(*u_double)*2);    
    u_double[0] = malloc(sizeof(**u_double)*weight_local*height_local); // free ?
    u_double[1] = malloc(sizeof(**u_double)*weight_local*height_local); // free ?
    
    for ( i=0 ; i<weight_local*height_local ; i++) {
      u_double[0][i] = 0.0;
      u_double[1][i] = 0.0;
    }
    
    printf("myrank:%d || my_row:%d || grid_rank:%d || my_coords:%d;%d || sixe:%d/%d \n", myrank, my_row, grid_rank, my_coords[0], my_coords[1], weight_local, height_local);


    // Init Communication

    MPI_Request sendRequests[2][4];
    MPI_Request recvRequests[2][4];
    MPI_Status  statusRequests[4];

    int otherCoord[2];    
    int otherRank;
    
    // 1. création du type
    MPI_Datatype column_type;
    MPI_Type_vector(height_local, 1, weight_local, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);
    
    for ( i=0 ; i<2 ; i++) {
      //DEBUG("horizontal comm");
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
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][RIGHT]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][RIGHT]);
	
	sendRequests[i][LEFT]   = NULL;
	recvRequests[i][LEFT]   = NULL;	
      }
      else if (position[RIGHT]) {
	// with left
	otherCoord[0] = my_coords[0]-1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][LEFT]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][LEFT]);

	sendRequests[i][RIGHT] = NULL;
	recvRequests[i][RIGHT] = NULL;
      }
      else {
	// with left
	otherCoord[0] = my_coords[0]-1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][LEFT]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][LEFT]);
	
	// with right
	otherCoord[0] = my_coords[0]+1;
	otherCoord[1] = my_coords[1];
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &sendRequests[i][RIGHT]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, MPI_DOUBLE, otherRank, WORK_TAG, grid_2D, &recvRequests[i][RIGHT]);
	
      }
      
      //DEBUG("vertical comm");    
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
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][DOWN]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][DOWN]);
	
	sendRequests[i][UP]   = NULL;
	recvRequests[i][UP]   = NULL;	
      }
      else if (position[DOWN]) {
	// with up
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]-1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][UP]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][UP]);

	sendRequests[i][DOWN] = NULL;
	recvRequests[i][DOWN] = NULL;
      }
      else {
	// with up
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]-1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][UP]);
	MPI_Recv_init(&u_double[i][1],              weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][UP]);

	// with down
	otherCoord[0] = my_coords[0];
	otherCoord[1] = my_coords[1]+1;
	MPI_Cart_rank(grid_2D, otherCoord, &otherRank);  
	MPI_Send_init(&u_double[i][weight_local*(height_local-2)+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &sendRequests[i][DOWN]);
	MPI_Recv_init(&u_double[i][weight_local*(height_local-1)+1], weight_local-2, column_type, otherRank, WORK_TAG, grid_2D, &recvRequests[i][DOWN]);
      }
      
    }

    
    // Init compute 
    DEBUG("I start to work");

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

    while (!stop) {
      //      printf("%d: round %d\n", grid_rank, nb_tour);
      u = u_double[nb_tour%2];
      v = u_double[(nb_tour+1)%2];
      
      norme = 0;
      // Compute border
      
      for (y=0 ; y<2 ; y++) {
	for (i=1; i<weight_local-1 ; i++) {
	  j = 1 + (height_local-3)*y;
	  
	  indice = i + weight_local*j;
	  v[indice] = 0.25*(u[	indice-weight_local] + u[indice+weight_local] + u[indice-1] + u[indice+1] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = v[indice] - u[indice];
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	  else if (norme < -norme_tmp )
	    norme = -norme_tmp; 	  
	}
      }
      
      // Communicate now TODO TODO !!!!
      
      
      // Compute interior
      for (j=y_min+1 ; j<y_max ; ++j){
	for (i=x_min+1 ; i<x_max ; ++i){
	  indice = i + weight_local*j;
	  v[indice] = 0.25*(u[indice-weight_local] + u[indice+weight_local] + u[indice-1] + u[indice+1] - hCarre*f(i*h,j*h));
	  
	  // On calcul la norme
	  norme_tmp = v[indice] - u[indice];
	  if (norme < norme_tmp)
	    norme = norme_tmp;
	  else if (norme < -norme_tmp )
	    norme = -norme_tmp;
	  //	  printf("*** tour:%d || work_rank:%d || norme_tmp:%lf\n",nb_tour,work_rank,norme_tmp);
	} 
      } 
     
      
      // Communicate
      for (i=0 ; i<2 ; i++) {
	if (sendRequests[(nb_tour+1)%2][i] != NULL)
	  MPI_Start(&sendRequests[(nb_tour+1)%2][i]);
	if (recvRequests[(nb_tour+1)%2][i] != NULL)
	  MPI_Start(&recvRequests[(nb_tour+1)%2][i]);
      }

      // Wait end comm
      
      //      MPI_Waitall(4,sendRequests[(nb_tour+1)%2],statusRequests);
      //MPI_Waitall(4,recvRequests[(nb_tour+1)%2],statusRequests);
      
      
      for (i=0;i<2;i++){
	if(sendRequests[(nb_tour+1)%2][i] != NULL)	
	  MPI_Wait(&sendRequests[(nb_tour+1)%2][i], statusRequests);	
	if(recvRequests[(nb_tour+1)%2][i] != NULL)	
	  MPI_Wait(&recvRequests[(nb_tour+1)%2][i], statusRequests);	
      }





      // Continue ??
      stop = norme < eps || nb_tour > max_round;
      //printf("*** tour:%d || work_rank:%d || norme:%lf\n",nb_tour,work_rank,norme);
      MPI_Allreduce(&stop,&stop_tmp,1,MPI_INT,MPI_LAND,grid_2D);
      stop = stop_tmp;
      
      
      if (!grid_rank) {
	printf("Tour:%d, finish with norme = %lf\n",nb_tour,norme);
      }
      

      nb_tour++;
      } /* end while(!stop) */

    // Check results
    
    DEBUG("I have finish my work");
    
    int isValid = 1;
        
    x = my_coords[0]*(matrix_size/horizontal_processor_number) ;
    y = my_coords[1]*(matrix_size/vertical_processor_number) ; 
    //    printf("(%d,%d)  ||  height_local:%d || weight_local:%d\n", my_coords[0],my_coords[1],height_local,weight_local);
    for (j=1 ; j<height_local-2 && isValid ; j++) {
      for (i=1 ; i<weight_local-2 && isValid; i++) {
	if (v[i + j*weight_local] - solution(h*(x+i),h*(y+j)) > eps)
	  isValid = 0;
	if (-v[i + j*weight_local] + solution(h*(x+i),h*(y+j)) > eps)
	  isValid = 0;

      }
    }

    /*
    if (isValid) 
      printf("%d says: Valid Result.\n", grid_rank);
    else
      printf("%d says: False Result\n", grid_rank);
    */

    int isAllValid = 0;
    MPI_Allreduce(&isValid,&isAllValid,1,MPI_INT,MPI_LAND,grid_2D);
    
    if (grid_rank==0) {
      if (isAllValid) 
	printf("Valid Result.\n");
      else
	printf("False Result\n");
    }
    

  } /* end if(my_row==0) */ 
  MPI_Finalize();
  return EXIT_SUCCESS;
}
