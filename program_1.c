#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct Grid_boxes{
  int box_id;
  int temp;
  int top_n;
  int bot_n;
  int left_n;
  int right_n;
  int xc;
  int yc;
  int height;
  int width;
  int *top_list;
  int *bot_list;
  int *left_list;
  int *right_list;
} Grid_box;

char* strsinline(char *){
    
}

int main(int argc, char *argv[])
{
  char *newline;
  int total_boxes = 0;
  int index = 0;
  int k=0;
  int row = 0;
  int col = 0;
  char line[256];
  int i=0;
  char delim[] = " ";
  
  //reading first char
  if(fgets(line, sizeof(line), stdin)){
    
    int n = strlen(line);

	char *ptr = strtok(line, delim);

	while(ptr != NULL)
	{
		//printf("%s\n", ptr);
		if(ptr && i==0){
		    total_boxes = (int) strtol(ptr, (char **)NULL, 10);
		}else if(ptr && i==1){
		    row = (int) strtol(ptr, (char **)NULL, 10);
		}else if(ptr){
		    col = (int) strtol(ptr, (char **)NULL, 10);
		}
		i++;
		ptr = strtok(NULL, delim);
		
	}

  }
  i=0;
  printf("total boxes are  %d\n", total_boxes);
  Grid_box *grid_boxes = malloc(sizeof(Grid_box) * total_boxes);

  
  while (fgets(line, sizeof(line), stdin)) {
      printf("%s\n", line);
      int n = strlen(line);

      char *ptr = strtok(line, delim);
    
      while(ptr != NULL)
      {
    	//printf("%s\n", ptr);
    	if(ptr && i==0){
    		   total_boxes = (int) strtol(ptr, (char **)NULL, 10);
    	}else if(ptr && i==1){
    	    row = (int) strtol(ptr, (char **)NULL, 10);
    	}else if(ptr){
    	    col = (int) strtol(ptr, (char **)NULL, 10);
    	}
    	i++;
    	ptr = strtok(NULL, delim);
    		
     }
  }
  

  

  printf("\n");

  return 0;

}
