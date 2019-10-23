#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <time.h>
//#include <float.h>

#define DBL_MAX 1.7976931348623158e+308 /* max value */
#define DBL_MIN 2.2250738585072014e-308 /* min positive value */
#define BILLION 1000000000L

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

int main(int argc, char *argv[])
{
  int total_boxes = 0;
  int row = 0;
  int col = 0;
  char line[500];
  int linecounter = 0;
  char delim[] = " \t";
  
  int i=0;
  int j=0;
  int k=0;
  
  double epsilon = 0.1;
  double affect_rate = 0.1;
  double cur_min_dsv = DBL_MAX;
  double cur_max_dsv = DBL_MIN;
  double *dsv_c = NULL;
  struct timespec start, end;
  double timediff;

  if(argc != 3){
    printf("Please provide correct number of arguments\n");
    exit(0);
  }

  sscanf(argv[1], "%lf", &affect_rate);
  sscanf(argv[2], "%lf", &epsilon);
  //printf("affec rate epsilon %lf %lf\n", affect_rate, epsilon);
  //scanf("%lf", &epsilon);
  
  //reading first line containng number of boxes, rows and cols
  if(fgets(line, sizeof(line), stdin)){
    
    //int n = strlen(line);
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
  dsv_c = malloc(sizeof(double) * total_boxes);
  int t=0;
  
  while (fgets(line, sizeof(line), stdin)) {
      
      //int n = strlen(line);
      if(emptyline(line)) continue;
      //printf("Line with chars  -- %s\n", line);
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
       // printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
          {
        //  printf("linecounter 2 %s\n", ptr);
          if(ptr && i==0){
            gb.top_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.top_list = malloc(sizeof(int)*(gb.top_n));
          }else if(ptr && i>=1 && j < gb.top_n){
             gb.top_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("top neighbor of %d is %d \n", gb.box_id, gb.top_list[j]);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 3){
        //printf("%s\n", ptr);
        j=0;
        i=0;
        while(ptr != NULL)
        {
        //  printf("linecounter 3 %s\n", ptr);
          if(ptr && i==0){
            gb.bot_n = (int) strtol(ptr, (char **)NULL, 10);
            gb.bot_list = malloc(sizeof(int)*(gb.bot_n));
          }else if(ptr && i>=1 &&j < gb.bot_n){
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
          }else if(ptr && i>=1  &&j < gb.left_n){
             gb.left_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("left neighbor of %d is %d \n", gb.box_id, gb.left_list[j]);
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
          }else if(ptr && i>=1  &&j < gb.right_n){
             gb.right_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             //printf("right neighbor of %d is %d \n", gb.box_id, gb.right_list[j]);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 6){
          //printf("temprature %s\n", ptr);
          sscanf(ptr, "%lf", &gb.temp);
          //gb.temp = (double) strtol(ptr, (char **)NULL, 10);
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
  
  printf("total boxes again %d\n", t);
  //printing all the grid boxes
  /*
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
  */
  
  //converging values from here
  printf("total boxes %d\n", t);
  printf("Max temprature %lf\n", cur_max_dsv);
  printf("Min temprature %lf\n", cur_min_dsv);

  int imax(int a, int b){
    return a>b ? a : b;
  }

  int imin(int a, int b){
    return a<b ? a : b;
  }
  
  int total_iterations = 0;
  
  clock_gettime(CLOCK_REALTIME,& start);
  clock_t start_clock, end_clock;
  start_clock = clock();

  while(1){
      total_iterations++;

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

          if(grid_boxes[cur].top_n == 0){
              //dsv_c[cur] += grid_boxes[cur].temp;
          }else{
              box_peri += cw;
              for(int tn = 0; tn<grid_boxes[cur].top_n; tn++){
                cur_box = grid_boxes[cur].top_list[tn];
                ov_start = imax(grid_boxes[cur_box].xc, cxc);
                ov_end = imin(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
                overlap = ov_end - ov_start;
                //printf("Top Neighbour and temp overlap %d %d %lf\n", overlap, cur_box, grid_boxes[cur_box].temp);
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
                
            }
          }
          
          //right neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].right_n == 0){
            //dsv_c[cur] += grid_boxes[cur].temp;
          }else{
            box_peri += ch;
            for(int rn = 0; rn<grid_boxes[cur].right_n; rn++){
                cur_box = grid_boxes[cur].right_list[rn];
                ov_start = imax(grid_boxes[cur_box].yc, cyc);
                ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
                overlap = ov_end - ov_start;
                //printf("right Neighbour and temp overlap %d %d %lf\n", overlap, cur_box, grid_boxes[cur_box].temp);
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
            }
        }
          
          //bottom neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].bot_n == 0){
            //dsv_c[cur] += grid_boxes[cur].temp;
          }else{
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
          }
          
          //left neighbours
          cur_box = 0; 
          overlap = 0;
          ov_end = 0;
          ov_start = 0;

          if(grid_boxes[cur].left_n == 0){
            //dsv_c[cur] += grid_boxes[cur].temp;
          }else{
            box_peri += ch;
            for(int ln = 0; ln<grid_boxes[cur].left_n; ln++){
                cur_box = grid_boxes[cur].left_list[ln];
                ov_start = imax(grid_boxes[cur_box].yc, cyc);
                ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
                overlap = ov_end - ov_start;
                //printf("left Neighbour and temp overlap %d %d %lf\n", overlap, cur_box, grid_boxes[cur_box].temp);
                dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
                if(overlap <= 0){
                  //printf("Something wrong with overlap \n");
                }
            }
          }

          double offset = 0;

          double cur_temp = grid_boxes[cur].temp;
          if(box_peri > 0){
            double avg_dsv = dsv_c[cur]/(double)box_peri;
            offset = ((cur_temp - avg_dsv)*affect_rate);
          }
          dsv_c[cur] = cur_temp - offset;

          /*
          if(avg_dsv > cur_temp){
            //grid_boxes[cur].temp = cur_temp + (avg_dsv - cur_temp)*affect_rate;
            dsv_c[cur] = cur_temp + ((avg_dsv - cur_temp)*affect_rate);
          }else{
            //grid_boxes[cur].temp = cur_temp - (cur_temp - avg_dsv)*affect_rate;
            dsv_c[cur] = cur_temp - ((cur_temp - avg_dsv)*affect_rate);
          }
          */
          //printf("Cur temprature %d : %lf\n", cur, dsv_c[cur]);
      }

      cur_min_dsv =  dsv_c[0];
      cur_max_dsv =  dsv_c[0];
      grid_boxes[0].temp = dsv_c[0];

      for(int curx=1; curx<total_boxes; curx++){
          grid_boxes[curx].temp = dsv_c[curx];
          cur_max_dsv = max(cur_max_dsv,  dsv_c[curx]);
          cur_min_dsv = min(cur_min_dsv,  dsv_c[curx]);
      }
      printf("Max temprature :   %lf\n", cur_max_dsv);
      printf("Min temprature :   %lf\n", cur_min_dsv);
      printf("loop counter :   %d\n", total_iterations);

      int diff = (cur_max_dsv - cur_min_dsv) <= (epsilon*cur_max_dsv) ? 1 : 0;
      if(diff==1) break;
      //if(cur_max_dsv == 0 && cur_min_dsv == 0) break;
     // if(cur_max_dsv - cur_min_dsv < 0.1) break;
  }

  clock_gettime(CLOCK_REALTIME,& end);
  end_clock = clock() - start_clock;
  //BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  timediff = (double)( ((end.tv_sec - start.tv_sec)*CLOCKS_PER_SEC) + ((end.tv_nsec -start.tv_nsec)/1000000) );
  printf("time taken -- time %lf\n", timediff);
  printf("time taken -- clock %ld\n", end_clock);
  printf("Total iterations for converging: %d", total_iterations);
  
  printf("\n");
  return 0;
}
