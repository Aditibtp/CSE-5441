#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/time.h>

typedef struct Grid_boxes{
  int box_id;
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

#define DBL_MAX 1.7976931348623158e+308 /* max value */
#define DBL_MIN 2.2250738585072014e-308 /* min positive value */

int total_boxes = 0;
Grid_box *grid_boxes;
Grid_box *d_grid_boxes;
double *dsv_c = NULL;
double *d_dsv_c = NULL;
double epsilon = 0.1;
double affect_rate = 0.1;
double cur_min_dsv = DBL_MAX;
double cur_max_dsv = DBL_MIN;

double *h_dsv_c = NULL;

int* countGflop = 0;
int* d_countGflop = 0;



// define thread hierarchy
int num_blocks = 8;
int num_th_per_blk = 1024;


int emptyline(char *line);


void printBoxes();

int emptyline(char *line){
  int len = strlen(line);
  for(int i=0; i<len; i++){
    if(!isspace(line[i])){
      return 0;
    }
  }
  return 1;
}

__device__ int imax(int a, int b){
  return a>b ? a : b;
}

__device__ int imin(int a, int b){
  return a<b ? a : b;
}

//printing all the grid boxes

void printBoxes(){
    for(int i=0; i<total_boxes; i++){
      printf("box id %d\n", grid_boxes[i].box_id);
      printf("box temprature %lf\n", h_dsv_c[i]);
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

__device__ void calculateDsvForBox(int box_index, Grid_box *grid_boxes, double *old_dsv, double *dsv_c, int total_boxes, double affect_rate, double epsilon, int* countGflop){

  int cur = box_index;
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
        dsv_c[cur] += (overlap*old_dsv[cur_box]);
        countGflop[0] +=2;
    }
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
        dsv_c[cur] += (overlap*old_dsv[cur_box]);
        countGflop[0] +=2;
    }
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
        dsv_c[cur] += (overlap*old_dsv[cur_box]);
        countGflop[0] +=2;
    }
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
        dsv_c[cur] += (overlap*old_dsv[cur_box]);
        countGflop[0] +=2;
    }
  }
  double offset = 0;

  double cur_temp = old_dsv[cur];
  if(box_peri > 0){
    double avg_dsv = dsv_c[cur]/(double)box_peri;
    offset = ((cur_temp - avg_dsv)*affect_rate);
    countGflop[0] +=3;
  }
  dsv_c[cur] = cur_temp - offset;
}

__global__ void Compute_dsv(Grid_box *grid_boxes, double *old_dsv, double *dsv_c, int total_boxes, double affect_rate, double epsilon, int* countGflop){
  
  int t_id = blockIdx.x * blockDim.x + threadIdx.x;
  if(t_id < total_boxes){
    calculateDsvForBox(t_id, grid_boxes, old_dsv, dsv_c, total_boxes, affect_rate, epsilon, countGflop);
  }
}

int main(int argc, char *argv[]){
  
  char line[500];
  int linecounter = 0;
  char delim[] = " \t";
  
  int i=0;
  int j=0;
  //int k=0;
  
  struct timespec start, end;
  double timediff;

  if(argc != 3){
    printf("Please provide correct number of arguments in the following order: <AFFECT_RATE> <EPSILON> <Number of threads> <INPUT_FILE>\n");
    exit(0);
  }
  
  sscanf(argv[1], "%lf", &affect_rate);
  sscanf(argv[2], "%lf", &epsilon);
  
  //reading first line containng number of boxes, rows and cols
  if(fgets(line, sizeof(line), stdin)){
    i=0;
    char *ptr = strtok(line, delim);
    //int row = 0;
    //int col = 0;
    while(ptr != NULL)
    {
      //printf("%s\n", ptr);
      if(ptr && i==0){
          total_boxes = (int) strtol(ptr, (char **)NULL, 10);
      }else if(ptr && i==1){
          //row = (int) strtol(ptr, (char **)NULL, 10);
      }else if(ptr){
          //col = (int) strtol(ptr, (char **)NULL, 10);
      }
      i++;
      ptr = strtok(NULL, delim);
      
    }

  }
  
  //do all the mallocs
  grid_boxes = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
  dsv_c = (double*)malloc(sizeof(double) * total_boxes);
  h_dsv_c = (double*)malloc(sizeof(double) * total_boxes);
  int t=0;
  
  printf("Startting to parse files\n");
  
  while (fgets(line, sizeof(line), stdin)) {
      
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
            gb.top_list = (int*)malloc(sizeof(int)*(gb.top_n));
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
            gb.bot_list =  (int*)malloc(sizeof(int)*(gb.bot_n));
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
            gb.left_list = (int*)malloc(sizeof(int)*(gb.left_n));
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
            gb.right_list = (int*)malloc(sizeof(int)*(gb.right_n));
          }else if(ptr && i>=1  &&j < gb.right_n){
             gb.right_list[j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
         }
      }else if(linecounter == 6){
          sscanf(ptr, "%lf", &h_dsv_c[t]);
          cur_max_dsv = h_dsv_c[t] > cur_max_dsv ? h_dsv_c[t] : cur_max_dsv;
          cur_min_dsv = h_dsv_c[t] < cur_min_dsv ? h_dsv_c[t] : cur_min_dsv;
      }
      linecounter++;
      if(linecounter == 7){
          grid_boxes[t] = gb;
          t++;
      }
      if(t==total_boxes)break;
  }
  
  int total_iterations = 0;
  
  
  printf("Total boxes %d\n", total_boxes);

  num_blocks = (total_boxes/512 + 1);
  
  dim3 threadsPerBlock(512, 1);
  dim3 blocksPerGrid(num_blocks + 1, 1);
  
  Grid_box *h_grid_boxes = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
  
  memcpy(h_grid_boxes, grid_boxes, total_boxes * sizeof(Grid_box));
  
  for(int cur=0; cur < total_boxes; cur++){
    //printf("cur index %d\n", cur);
    cudaMalloc(&(h_grid_boxes[cur].left_list), (grid_boxes[cur].left_n)*sizeof(int));
    cudaMemcpy(h_grid_boxes[cur].left_list, grid_boxes[cur].left_list,  (grid_boxes[cur].left_n)*sizeof(int), cudaMemcpyHostToDevice);
    
    cudaMalloc(&(h_grid_boxes[cur].right_list), (grid_boxes[cur].right_n)*sizeof(int));
    cudaMemcpy(h_grid_boxes[cur].right_list, grid_boxes[cur].right_list,  (grid_boxes[cur].right_n)*sizeof(int), cudaMemcpyHostToDevice);
    
    cudaMalloc(&(h_grid_boxes[cur].bot_list), (grid_boxes[cur].bot_n)*sizeof(int));
    cudaMemcpy(h_grid_boxes[cur].bot_list, grid_boxes[cur].bot_list,   (grid_boxes[cur].bot_n)*sizeof(int), cudaMemcpyHostToDevice);
    
    cudaMalloc(&(h_grid_boxes[cur].top_list), ( grid_boxes[cur].top_n)*sizeof(int));
    cudaMemcpy(h_grid_boxes[cur].top_list, grid_boxes[cur].top_list, ( grid_boxes[cur].top_n)*sizeof(int), cudaMemcpyHostToDevice);

  }

  countGflop = (int*) malloc(sizeof(int));
  countGflop[0] = 1;

  double *d_old_dsv = NULL;

  if(cudaMalloc(&d_old_dsv, sizeof(double)*total_boxes) != cudaSuccess){
    printf("Problem with memory allocation for old dsv\n");
    return 0;
  }

  if(cudaMalloc(&d_countGflop, sizeof(int)) != cudaSuccess){
    printf("Problem with memory allocation for gflop\n");
    return 0;
  }

  if(cudaMalloc(&d_grid_boxes, sizeof(Grid_box)*total_boxes) != cudaSuccess){
    printf("Problem with memory allocation grid boxes\n");
    return 0;
  }
  if(cudaMalloc(&d_dsv_c, sizeof(double)*total_boxes) != cudaSuccess){
    printf("Problem with memory allocation dsv_c\n");
    return 0;
  }
  printf("Done with mallocs and copies\n");

  if(cudaMemcpy(d_countGflop, countGflop, sizeof(int), cudaMemcpyHostToDevice) != cudaSuccess){
    printf("Problem with copying from host to device gflop\n");
    return 0;
  }

  if(cudaMemcpy(d_grid_boxes, h_grid_boxes, sizeof(Grid_box)*total_boxes, cudaMemcpyHostToDevice) != cudaSuccess){
    printf("Problem with copying from host to device grid_box\n");
    return 0;
  }
  
  if(cudaMemcpy(d_old_dsv, h_dsv_c, sizeof(double)*total_boxes, cudaMemcpyHostToDevice) != cudaSuccess){
        printf("Problem with copying from host to device old dsv\n");
        return 0;
   }

  printf("No faults till here \n");

  int gflopcounter = 0;

  //struct timeval t_start, t_end, t_diff;
  clock_gettime(CLOCK_REALTIME,& start);

  clock_t start_clock, end_clock;
  start_clock = clock();

  time_t time_t_start;
  time_t_start = time(NULL); 
  while(1){
      total_iterations++;

      Compute_dsv<<<(total_boxes/num_th_per_blk+1), num_th_per_blk>>>(d_grid_boxes, d_old_dsv, d_dsv_c,
                       total_boxes, affect_rate, epsilon, d_countGflop);
   
      cudaDeviceSynchronize();
      if(cudaMemcpy(dsv_c, d_dsv_c, sizeof(double)*total_boxes, cudaMemcpyDeviceToHost) != cudaSuccess){
        printf("Problem with copying from device to host dsv_c\n");
        return 0;
      }

      if(cudaMemcpy(countGflop, d_countGflop, sizeof(int), cudaMemcpyDeviceToHost) != cudaSuccess){
        printf("Problem with copying from device to host gflop\n");
        return 0;
      }
      //printf("No faults till here with synchronise\n");
      cudaDeviceSynchronize();

      cur_min_dsv =  dsv_c[0];
      cur_max_dsv =  dsv_c[0];
      h_dsv_c[0] = dsv_c[0];

      for(int curx=1; curx<total_boxes; curx++){
          h_dsv_c[curx] = dsv_c[curx];
          cur_max_dsv = max(cur_max_dsv,  dsv_c[curx]);
          cur_min_dsv = min(cur_min_dsv,  dsv_c[curx]);
      }


      gflopcounter += countGflop[0];

      if(cudaMemcpy(d_grid_boxes, h_grid_boxes, sizeof(Grid_box)*total_boxes, cudaMemcpyHostToDevice) != cudaSuccess){
        printf("Problem with copying from host to device grid_box\n");
        return 0;
      }

      if(cudaMemcpy(d_old_dsv, h_dsv_c, sizeof(double)*total_boxes, cudaMemcpyHostToDevice) != cudaSuccess){
        printf("Problem with copying from host to device old dsv\n");
        return 0;
      }

      countGflop[0] = 0;

      if(cudaMemcpy(d_countGflop, countGflop, sizeof(int), cudaMemcpyHostToDevice) != cudaSuccess){
        printf("Problem with copying from host to device grid_box\n");
        return 0;
      }
      //printf("No faults till here all copying finished\n");

      int diff = (cur_max_dsv - cur_min_dsv) <= (epsilon*cur_max_dsv) ? 1 : 0;
      if(diff==1){
        break;
      }

  }

  time_t time_t_end;
  time_t_end = time(NULL); 

  //double elapsed=0;
  clock_gettime(CLOCK_REALTIME,&end);
  end_clock = (double)((clock() - start_clock));
  
  timediff = (double)((end.tv_sec - start.tv_sec)*CLOCKS_PER_SEC + ((end.tv_nsec -start.tv_nsec)/1000000));

  printf("\n********************************************************************************\n");
  printf("Total number of blocks created %d,\n", total_boxes/num_th_per_blk+1);
  printf("Total number of threads per block %d,\n", num_th_per_blk);
  printf("dissipation converged in %d iterations,\n", total_iterations);
  printf("\twith max DSV = %lf and min DSV = %lf\n", cur_max_dsv, cur_min_dsv);

  printf("\taffect rate = %lf; epsilon = %lf\n\n", affect_rate, epsilon);
  printf("elapsed convergence loop time (clock_gettime()): %lf\n", timediff);
  printf("elapsed convergence loop time (clock): %ld\n", end_clock);
  printf("elapsed convergence loop time (time_t): %ld\n", (time_t_end - time_t_start));
  printf("Total Gflop operations:   %d\n", gflopcounter);
  if((time_t_end - time_t_start) > 0){
    printf("Gflop/sec operations:   %d\n", (gflopcounter/(time_t_end - time_t_start)));
  }
  printf("\n********************************************************************************\n");

  //freeing up memory
  for(int i=0; i<total_boxes; i++){
      
      free(grid_boxes[i].left_list);
      
      free(grid_boxes[i].right_list);

      free(grid_boxes[i].top_list);

      free(grid_boxes[i].bot_list);
      
  }

  for(int cur=0; cur < total_boxes; cur++){
    cudaFree(h_grid_boxes[cur].left_list);
    cudaFree(h_grid_boxes[cur].right_list);
    cudaFree(h_grid_boxes[cur].bot_list);
    cudaFree(h_grid_boxes[cur].top_list);
  }
  free(grid_boxes);
  free(dsv_c);
  free(countGflop);
  cudaFree(d_countGflop);
  cudaFree(d_dsv_c);
  cudaFree(d_grid_boxes);


  return 0;
}
