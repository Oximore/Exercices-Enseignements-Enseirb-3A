#include <stdio.h>
#include <stdlib.h>

#include "test_code.h"

int main (int argc, char* argv[]) {
  
  int i,j,n;
  double  *u, *v, *u_tmp;
  double h, hCarre, eps, norme, norme_tmp;
  int tour_max, nb_tour;
  /* Arg filter */
  if (argc !=3) {
    printf("usage : %s eps size_matrix\n",argv[0]);
    return EXIT_FAILURE;
  }
  
  i = 0;
  sscanf(argv[++i],"%lf",&eps);
  n = atoi(argv[++i]);
  if (n<2) {
    printf("n<2\n");
    return EXIT_FAILURE;
  }
  tour_max = 100; 
  
  /* print */
  printf("n=%d eps=%lf tour_max=%d\n",n,eps,tour_max);

  /* Init */
  h = 1.0/(n-1);
  hCarre = h*h;
  
  int total_size = n*n;
  u = malloc(sizeof(*u)*total_size);
  v = malloc(sizeof(*u)*total_size);
  
  for ( j=0 ; j<n-2 ; ++j){
    for ( i=0 ; i<n-2 ; ++i){
      u[i + n*j] = 0;
      v[i + n*j] = 0;
    } 
  } 
  
  
  /* Compute */
  
  nb_tour = 0;
  norme = eps + 1;
  // on ne dépasse pas le nombre de tour maximum ET on s'arret lorsque la norme est assez petite
  while (++nb_tour < tour_max && norme > eps) {
    
    printf("Round %d\n",nb_tour);
 
    norme = 0;
    for ( j=1 ; j<n-1 ; ++j){
      for ( i=1 ; i<n-1 ; ++i){
	
	v[i + n*j] = 0.25*(u[(i-1) + n*j] + u[(i+1) + n*j] + u[i + n*(j-1)] + u[i + n*(j+1)] - hCarre*f(i*h,j*h));
	
	// On calcul la norme
	norme_tmp = v[i + n*j] - u[i + n*j];
	if (norme < norme_tmp)
	  norme = norme_tmp;
	else if (norme < -norme_tmp )
	  norme = -norme_tmp;
      } 
    } 
    
    // on échange u et v;
    u_tmp = u;
    u = v;
    v = u_tmp;

  }
  
  printf("Finish with norme = %lf\n",norme);
  /*
  for ( j=1 ; j<n-1 ; ++j){
    for ( i=1 ; i<n-1 ; ++i){
      printf("(%lf-%lf)\t", u[i + n*j] , solution(h*i, h*j));
    }
    printf("\n");
  }
  */

  return EXIT_SUCCESS;
}


 
