#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 8
static long num_steps = 100000; double step; 
 
void main () {
  double  x, pi, sum[NUM_THREADS]; 
  step = 1.0/(double) num_steps; 
  int actual_number_of_threads = 0;
  
  #pragma omp parallel private (  x )
  {	  
  // Get the number of thread and send its data.
    int thread_id = omp_get_thread_num(); 
    // See the actual number of threads created.
    if (thread_id == 0) {
      // Let the master thread update the
      // actual number of threads created.
      actual_number_of_threads = omp_get_num_threads();
      printf("actual threads %d\n", actual_number_of_threads);
    }
    int id = omp_get_thread_num(); 
    int i = 0;
    for (i=id, sum[id]=0.0; i< num_steps; i=i+NUM_THREADS)
    {
      x = (i+0.5)*step; 
      sum[id] += 4.0/(1.0+x*x); 
    } 
  } 
  int i = 0;
  for(i=1; i<NUM_THREADS; i++)
    sum[0] += sum[i];  pi = sum[0] / num_steps;
  printf("pi = %6.12f\n", pi);
}
