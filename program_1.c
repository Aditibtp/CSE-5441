#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <float.h>

//#define DBL_MAX 1.7976931348623158e+308 /* max value */
//#define DBL_MIN 2.2250738585072014e-308 /* min positive value */

typedef struct Grid_boxes{
  int box_id;
  double temp;
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

double max(double a, double b){
    return a > b ? a : b;
}

double min(double a, double b){
    return a < b ? a : b;
}

int main(int argc, char *argv[])
{
  int total_boxes = 0;
  int row = 0;
  int col = 0;
  char line[500];
  int linecounter = 0;
  char delim[] = " ";
  
  int i=0;
  int j=0;
  int k=0;
  
  double epsilon = 0.1;
  double affect_rate = 0.1;
  double cur_min_dsv = DBL_MAX;
  double cur_max_dsv = DBL_MIN;
  
  //reading first line containng number of boxes, rows and cols
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
  int t=0;
  
  while (fgets(line, sizeof(line), stdin)) {
      //printf("%s\n", line);
      int n = strlen(line);
      Grid_box gb;
      i=0;
      char *ptr = strtok(line, delim);
      if(linecounter == 7) linecounter = 0;
      if(linecounter == 0){
          //printf("%s\n", ptr);
          gb.box_id = (int) strtol(ptr, (char **)NULL, 10);
          
      }else if(linecounter == 1){
        //printf("%s\n", ptr);
        i=0;
        while(ptr != NULL)
          {
        //  printf("linecounter 1 %s\n", ptr);
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
       // printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
          {
        //  printf("linecounter 2 %s\n", ptr);
          if(ptr && i==0){
            gb.top_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.top_list = malloc(sizeof(int)*(gb.top_n));
          }else if(ptr && i==1){
             gb.top_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 3){
        printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
          {
        //  printf("linecounter 3 %s\n", ptr);
          if(ptr && i==0){
            gb.bot_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.bot_list = malloc(sizeof(int)*(gb.bot_n));
          }else if(ptr && i==1){
             gb.bot_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("bottom neighbor of %d is %d \n", gb.box_id, gb.bot_list[j]);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 4){
        //printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
          {
        //  printf("linecounter 4 %s\n", ptr);
          if(ptr && i==0){
            gb.left_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.left_list = malloc(sizeof(int)*(gb.left_n));
          }else if(ptr && i==1){
             gb.left_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("bottom neighbor of %d is %d \n", gb.box_id, gb.bot_list[j]);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 5){
       // printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
          {
          //printf("linecounter 5 %s\n", ptr);
          if(ptr && i==0){
            gb.right_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.right_list = malloc(sizeof(int)*(gb.right_n));
          }else if(ptr && i==1){
             gb.right_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("bottom neighbor of %d is %d \n", gb.box_id, gb.right_list[j]);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 6){
          //printf("temprature %s\n", ptr);
          gb.temp = (double) strtol(ptr, (char **)NULL, 10);
          cur_max_dsv = gb.temp > cur_max_dsv ? gb.temp : cur_max_dsv;
          cur_min_dsv = gb.temp < cur_min_dsv ? gb.temp : cur_min_dsv;
      }
      linecounter++;
      if(linecounter == 7){
          grid_boxes[t] = gb;
          t++;
      }
  }
  
  //printing all the grid boxes
  /*
  for(int i=0; i<total_boxes; i++){
      printf("box id %d\n", grid_boxes[i].box_id);
      printf("box temprature %d\n", grid_boxes[i].temp);
      printf("box x %d\n", grid_boxes[i].xc);
      printf("box y %d\n", grid_boxes[i].yc);
      printf("box height and width %d    %d\n", grid_boxes[i].height, grid_boxes[i].width);
      
      printf("Left neighbours: ");
      for(int j=0; j<grid_boxes[i].left_n; j++){
        printf("%d ", grid_boxes[i].left_list[j]);
      }
      printf("\nRight neighbours: ");

      for(int j=0; j<grid_boxes[i].right_n; j++){
        printf("%d ", grid_boxes[i].right_list[j]);
      }
      printf("\ntop neighbours: ");

      for(int j=0; j<grid_boxes[i].top_n; j++){
        printf("%d ", grid_boxes[i].top_list[j]);
      }
      printf("\nbottom neighbours: ");

      for(int j=0; j<grid_boxes[i].bot_n; j++){
        printf("%d ", grid_boxes[i].bot_list[j]);
      }

      printf("\n*******\n");
      
  }
  */
  
  //converging values from here
  printf("Max temprature %f\n", cur_max_dsv);
  printf("Min temprature %f\n", cur_min_dsv);
  
  int total_iterations = 0;
  
  while((cur_max_dsv - cur_min_dsv)/cur_max_dsv > epsilon){
      total_iterations++;
      for(int cur = 0; cur < total_boxes; cur++){
          int cxc = grid_boxes[cur].xc;
          int cyc = grid_boxes[cur].yc;
          int ch = grid_boxes[cur].height;
          int cw = grid_boxes[cur].width;
          double dsv_c = 0;
          
          //top neighbours
          int cur_box = 0; 
          int overlap = 0;
          int ov_end = 0;
          int ov_start = 0;
          for(int tn = 0; tn<grid_boxes[cur].top_n; tn++){
              cur_box = grid_boxes[cur].top_list[tn];
              ov_start = max(grid_boxes[cur_box].xc, cxc);
              ov_end = min(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
              overlap = ov_end - ov_start;
              dsv_c += overlap*grid_boxes[cur_box].temp;
          }
          
          //right neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;
          for(int rn = 0; rn<grid_boxes[cur].right_n; rn++){
              cur_box = grid_boxes[cur].right_list[rn];
              ov_start = max(grid_boxes[cur_box].yc, cyc);
              ov_end = min(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
              overlap = ov_end - ov_start;
              dsv_c += overlap*grid_boxes[cur_box].temp;
          }
          
          //bottom neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;
          for(int bn = 0; bn<grid_boxes[cur].bot_n; bn++){
              cur_box = grid_boxes[cur].top_list[bn];
              ov_start = max(grid_boxes[cur_box].xc, cxc);
              ov_end = min(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
              overlap = ov_end - ov_start;
              dsv_c += overlap*grid_boxes[cur_box].temp;
          }
          
          //left neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;
          for(int ln = 0; ln<grid_boxes[cur].left_n; ln++){
              cur_box = grid_boxes[cur].right_list[ln];
              ov_start = max(grid_boxes[cur_box].yc, cyc);
              ov_end = min(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
              overlap = ov_end - ov_start;
              dsv_c += overlap*grid_boxes[cur_box].temp;
          }
          
          int box_peri = 2*cw + 2*ch;
          double avg_dsv = dsv_c/(double)box_peri;
          grid_boxes[cur].temp = grid_boxes[cur].temp - (grid_boxes[cur].temp - avg_dsv)*affect_rate;
      }
  }
  
  printf("Total iterations for converging: %d", total_iterations);
  
  printf("\n");
  return 0;
}
