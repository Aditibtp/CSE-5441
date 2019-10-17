/******************************************************************************
* FILE: hello.c
* DESCRIPTION:
*   A "hello world" Pthreads program.  Demonstrates thread creation and
*   termination.
* AUTHOR: Blaise Barney
* LAST REVISED: 08/09/11
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

typedef struct Grid_boxes{
  int box_id;
  int temp;
  int top_n;
  int bot_n;
  int left_n;
  int right_n;
  int *top_list;
  int *bot_list;
  int *left_list;
  int *right_list;
} Grid_box;



int main(int argc, char *argv[])
{
   /*pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(t=0;t<NUM_THREADS;t++){
     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
     }

   // Last thing that main() should do 
   pthread_exit(NULL);*/

  FILE *file;
  char filename[200];
  char *newline;
  int total_boxes = 0;
  int index = 0;
  int k=0;

  while(!feof(stdin) && k != -1){
    scanf("%d", &k);
    printf("some val %d\n", k);
    //arr[index] = k;
    index++;
  }



  //Grid_box *gbs = malloc(total_boxes * sizeof(Grid_box));

  //if(fgets(filename, sizeof(filename), stdin) == NULL)
    //return -1;
  //newline = strchr(filename, '\n');

  //if(newline)
    //*newline = '\0';
  //printf("****%s*****\n", filename);

  //file = fopen(filename, "r");

  

  printf("\n");

  return 0;

}