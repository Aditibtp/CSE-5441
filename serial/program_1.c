#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/time.h>
//#include <float.h>

#define DBL_MAX 1.7976931348623158e+308 /* max value */
#define DBL_MIN 2.2250738585072014e-308 /* min positive value */

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

int total_boxes = 0;
Grid_box *grid_boxes;
double *dsv_c = NULL;
double epsilon = 0.1;
double affect_rate = 0.1;

double max(double a, double b){
    return a > b ? a : b;
}

double min(double a, double b){
    return a < b ? a : b;
}

int emptyline(char *line){
  int len = strlen(line);
  //char *ptr = strtok(line, "delim");
 // printf("Checking line %s\n", line);
  for(int i=0; i<len; i++){
    if(!isspace(line[i])){
      return 0;
    }
  }
  return 1;
}

  //printf("total boxes again %d\n", t);
  //printing all the grid boxes

void printBoxes(){
    for(int i=0; i<total_boxes; i++){
      printf("box id %d\n", grid_boxes[i].box_id);
      printf("box temprature %lf\n", grid_boxes[i].temp);
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
}

int imax(int a, int b){
  return a>b ? a : b;
}

int imin(int a, int b){
  return a<b ? a : b;
}

void convergenceLoop(){

  for(int cur = 0; cur < total_boxes; cur++){

          int cxc = grid_boxes[cur].xc;
          int cyc = grid_boxes[cur].yc;
          int ch = grid_boxes[cur].height;
          int cw = grid_boxes[cur].width;
          dsv_c[cur] = 0;
          int box_peri = 0;
          
          //top neighbours
          int cur_box = 0; 
          int overlap = 0;
          int ov_end = 0;
          int ov_start = 0;

          if(grid_boxes[cur].top_n > 0){
              box_peri += cw;
              for(int tn = 0; tn<grid_boxes[cur].top_n; tn++){
                cur_box = grid_boxes[cur].top_list[tn];
                ov_start = imax(grid_boxes[cur_box].xc, cxc);
                ov_end = imin(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
                overlap = ov_end - ov_start;
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
                
            }
           // printf("top dsv_c[%d] :%lf \n",cur, dsv_c[cur] );
          }
          
          //right neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].right_n > 0){
            box_peri += ch;
            for(int rn = 0; rn<grid_boxes[cur].right_n; rn++){
                cur_box = grid_boxes[cur].right_list[rn];
                ov_start = imax(grid_boxes[cur_box].yc, cyc);
                ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
                overlap = ov_end - ov_start;
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
            }
            //printf("right dsv_c[%d] :%lf \n",cur, dsv_c[cur] );
        }
          
          //bottom neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].bot_n > 0){
            box_peri += cw;
            for(int bn = 0; bn<grid_boxes[cur].bot_n; bn++){
                cur_box = grid_boxes[cur].bot_list[bn];
                ov_start = imax(grid_boxes[cur_box].xc, cxc);
                ov_end = imin(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
                overlap = ov_end - ov_start;
                //printf("bottom Neighbour and temp overlap %d %d %lf\n", overlap, cur_box, grid_boxes[cur_box].temp);
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
            }
           // printf("bottom dsv_c[%d] :%lf \n",cur, dsv_c[cur] );
          }
          
          //left neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].left_n > 0){
            box_peri += ch;
            for(int ln = 0; ln<grid_boxes[cur].left_n; ln++){
                cur_box = grid_boxes[cur].left_list[ln];
                ov_start = imax(grid_boxes[cur_box].yc, cyc);
                ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
                overlap = ov_end - ov_start;
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
            }
           // printf("left dsv_c[%d] :%lf \n",cur, dsv_c[cur] );
          }

          double offset = 0;

        //  printf("box peri[i] : %d %d \n",cur, box_peri );

          double cur_temp = grid_boxes[cur].temp;
          if(box_peri > 0){
            double avg_dsv = dsv_c[cur]/(double)box_peri;
            offset = ((cur_temp - avg_dsv)*affect_rate);
          }
          dsv_c[cur] = cur_temp - offset;
      }
}

int main(int argc, char *argv[])
{
  
  int row = 0;
  int col = 0;
  char *line;
  int linecounter = 0;
  char delim[] = " \t";
  
  int i=0;
  int j=0;
  int k=0;
  
  double cur_min_dsv = DBL_MAX;
  double cur_max_dsv = DBL_MIN;
  struct timespec start, end;
  double timediff;

  if(argc != 3){
    printf("Please provide correct number of arguments in the following order: <AFFECT_RATE> <EPSILON> <INPUT_FILE>\n");
    exit(0);
  }

  sscanf(argv[1], "%lf", &affect_rate);
  sscanf(argv[2], "%lf", &epsilon);
  
  FILE * fp;
  fp = fopen("../inputs/testgrid_400_12206", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  
  
  size_t len = 0;
  ssize_t read;
      
  if ((read = getline(&line, &len, fp)) != -1){
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
  
  grid_boxes = malloc(sizeof(Grid_box) * total_boxes);
  dsv_c = malloc(sizeof(double) * total_boxes);
  int t=0;
  
  while ((read = getline(&line, &len, fp)) != -1) {
      
      if(emptyline(line)) continue;
      
      Grid_box gb;

      i=0;
      char *ptr = strtok(line, delim);
      if(linecounter == 7) linecounter = 0;
      if(linecounter == 0){
          
          gb.box_id = (int) strtol(ptr, (char **)NULL, 10);
          
      }else if(linecounter == 1){
        
        i=0;
        while(ptr != NULL)
          {
        
          if(ptr && i==0){
            gb.yc = (int) strtol(ptr, (char **)NULL, 10);
          }else if(ptr && i==1){
              gb.xc = (int) strtol(ptr, (char **)NULL, 10);
          }else if(i==2 && ptr){
              gb.height = (int) strtol(ptr, (char **)NULL, 10);
          }else if(ptr){
              gb.width = (int) strtol(ptr, (char **)NULL, 10);
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 2){
       
        j=0;
        i=0;
        while(ptr != NULL)
          {
          if(ptr && i==0){
            gb.top_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.top_list = malloc(sizeof(int)*(gb.top_n));
          }else if(ptr && i>=1 && j < gb.top_n){
             gb.top_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 3){
        j=0;
        i=0;
        while(ptr != NULL)
        {
          if(ptr && i==0){
            gb.bot_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.bot_list = malloc(sizeof(int)*(gb.bot_n));
          }else if(ptr && i>=1 &&j < gb.bot_n){
             gb.bot_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
        }
      }else if(linecounter == 4){
        j=0;
        i=0;
        while(ptr != NULL)
          {
         if(ptr && i==0){
            gb.left_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.left_list = malloc(sizeof(int)*(gb.left_n));
          }else if(ptr && i>=1  &&j < gb.left_n){
             gb.left_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 5){
        j=0;
        i=0;
        while(ptr != NULL)
          {
          if(ptr && i==0){
            gb.right_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.right_list = malloc(sizeof(int)*(gb.right_n));
          }else if(ptr && i>=1  &&j < gb.right_n){
             gb.right_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 6){
          sscanf(ptr, "%lf", &gb.temp);
          cur_max_dsv = gb.temp > cur_max_dsv ? gb.temp : cur_max_dsv;
          cur_min_dsv = gb.temp < cur_min_dsv ? gb.temp : cur_min_dsv;
      }
      linecounter++;
      if(linecounter == 7){
          grid_boxes[t] = gb;
          t++;
      }
      if(t==total_boxes)break;
  }
  
  //printBoxes(grid_boxes, total_boxes);
  
  int total_iterations = 0;

  struct timeval t_start, t_end, t_diff;
  clock_gettime(CLOCK_REALTIME,& start);
  clock_t start_clock, end_clock;
  start_clock = clock();
  gettimeofday(&t_start, NULL);
  unsigned long long startTime = ((unsigned long long)(t_start.tv_sec*CLOCKS_PER_SEC)) + ((unsigned long long)((t_start.tv_usec))/1000);

  while(1){
      total_iterations++;

      convergenceLoop();

      cur_min_dsv =  dsv_c[0];
      cur_max_dsv =  dsv_c[0];
      grid_boxes[0].temp = dsv_c[0];

      for(int curx=1; curx<total_boxes; curx++){
          grid_boxes[curx].temp = dsv_c[curx];
        //  printf("temp received %lf \n", grid_boxes[curx].temp);
          cur_max_dsv = max(cur_max_dsv,  dsv_c[curx]);
          cur_min_dsv = min(cur_min_dsv,  dsv_c[curx]);
      }

     // printf("\n");
      printf("cur min: %lf  and max   %lf \n",cur_min_dsv, cur_max_dsv);
      int diff = (cur_max_dsv - cur_min_dsv) <= (epsilon*cur_max_dsv) ? 1 : 0;
      if(diff==1) break;

  }

  gettimeofday(&t_end, NULL);
  unsigned long long endTime = ((unsigned long long)t_end.tv_sec)*CLOCKS_PER_SEC + ((unsigned long long)((t_end.tv_usec))/1000);

  double elapsed=0;
  clock_gettime(CLOCK_REALTIME,&end);
  end_clock = (double)((clock() - start_clock));
  
  elapsed = (endTime - startTime);
  timediff = (double)((end.tv_sec - start.tv_sec)*CLOCKS_PER_SEC + ((end.tv_nsec -start.tv_nsec)/1000000));

  printf("\n********************************************************************************\n");
  printf("dissipation converged in %d iterations,\n", total_iterations);
  printf("\twith max DSV = %lf and min DSV = %lf\n", cur_max_dsv, cur_min_dsv);

  printf("\taffect rate = %lf; epsilon = %lf\n\n", affect_rate, epsilon);
  printf("elapsed convergence loop time (clock_gettime()): %lf\n", timediff);
  printf("elapsed convergence loop time (clock): %ld\n", end_clock);
  printf("elapsed convergence loop time (gettimeofday()): %lf\n", elapsed);
  printf("\n********************************************************************************\n");

  //freeing up memory
  for(int i=0; i<total_boxes; i++){
      
      free(grid_boxes[i].left_list);
      
      free(grid_boxes[i].right_list);

      free(grid_boxes[i].top_list);

      free(grid_boxes[i].bot_list);
      
  }
  free(grid_boxes);

  return 0;
}
