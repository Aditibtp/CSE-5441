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

int main(int argc, char *argv[])
{
  char *newline;
  int total_boxes = 0;
  int index = 0;
  int k=0;
  int row = 0;
  int col = 0;
  char line[256];
  int linecounter = 0;
  char delim[] = " ";
  int i=0;
  int j=0;
  //reading first char
  if(fgets(line, sizeof(line), stdin)){
    
    int n = strlen(line);
    i=0;
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
  
  printf("total boxes are  %d\n", total_boxes);
  Grid_box *grid_boxes = malloc(sizeof(Grid_box) * total_boxes);

  
  while (fgets(line, sizeof(line), stdin)) {
      printf("%s\n", line);
      int n = strlen(line);
      Grid_box gb;
      i=0;
      char *ptr = strtok(line, delim);
      if(linecounter == 0){
          printf("%s\n", ptr);
          gb.box_id = (int) strtol(ptr, (char **)NULL, 10);
          
      }else if(linecounter == 1){
        printf("%s\n", ptr);
        while(ptr != NULL)
          {
        	//printf("%s\n", ptr);
        	if(ptr && i==0){
        		gb.xc = (int) strtol(ptr, (char **)NULL, 10);
        	}else if(ptr && i==1){
        	    gb.yc = (int) strtol(ptr, (char **)NULL, 10);
        	}else if(i==2 && ptr){
        	    gb.height = (int) strtol(ptr, (char **)NULL, 10);
        	}else if(ptr){
        	    gb.width = (int) strtol(ptr, (char **)NULL, 10);
        	}
        	i++;
        	ptr = strtok(NULL, delim);
        		
         }
      }else if(linecounter == 2){
          i=0;
          j=0;
          while(ptr != NULL)
          {
        	//printf("%s\n", ptr);
        	if(ptr && i==0){
        		gb.top_n = (int) strtol(ptr, (char **)NULL, 10);
        		gb.top_list = malloc(sizeof(int) * gb.top_n);
        	}else if(ptr){
        	    gb.top_list[j++] = (int) strtol(ptr, (char **)NULL, 10);
        	    j++;
        	}
        	i++;
        	ptr = strtok(NULL, delim);
        		
         }
      }else if(linecounter == 3){
          i=0;
          j=0;
          while(ptr != NULL)
          {
        	//printf("%s\n", ptr);
        	if(ptr && i==0){
        		gb.bot_n = (int) strtol(ptr, (char **)NULL, 10);
        		gb.bot_list = malloc(sizeof(int) * gb.bot_n);
        	}else if(ptr){
        	    gb.bot_list[j++] = (int) strtol(ptr, (char **)NULL, 10);
        	    j++;
        	}
        	i++;
        	ptr = strtok(NULL, delim);
        		
         }
      }else if(linecounter == 4){
          i=0;
          j=0;
          while(ptr != NULL)
          {
        	//printf("%s\n", ptr);
        	if(ptr && i==0){
        		gb.left_n = (int) strtol(ptr, (char **)NULL, 10);
        		gb.left_list = malloc(sizeof(int) * gb.left_n);
        	}else if(ptr){
        	    gb.left_list[j++] = (int) strtol(ptr, (char **)NULL, 10);
        	    j++;
        	}
        	i++;
        	ptr = strtok(NULL, delim);
        		
         }
      }else if(linecounter == 5){
          i=0;
          j=0;
          while(ptr != NULL)
          {
        	//printf("%s\n", ptr);
        	if(ptr && i==0){
        		gb.right_n = (int) strtol(ptr, (char **)NULL, 10);
        		gb.left_list = malloc(sizeof(int) * gb.right_n);
        	}else if(ptr){
        	    gb.right_list[j++] = (int) strtol(ptr, (char **)NULL, 10);
        	    j++;
        	}
        	i++;
        	ptr = strtok(NULL, delim);
        		
         }
      }else if(linecounter == 6){
          gb.temp = (int) strtol(ptr, (char **)NULL, 10);
      }
      linecounter++;
  }
  
  printf("\n");

  return 0;

}
