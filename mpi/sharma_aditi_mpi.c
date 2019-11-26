#include <pthread.h>
#include <omp.h>
#include <mpi.h>
#include "sharma_aditi_lab5.h"


double max(double a, double b){
    return a > b ? a : b;
}

double min(double a, double b){
    return a < b ? a : b;
}

int emptyline(char *line){
  int len = strlen(line);
  for(int i=0; i<len; i++){
    if(!isspace(line[i])){
      return 0;
    }
  }
  return 1;
}

int imax(int a, int b){
  return a>b ? a : b;
}

int imin(int a, int b){
  return a<b ? a : b;
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
        //printf("%d ", grid_boxes[i].left_list[][j]);
      }
      printf("\nRight neighbours: ");

      for(int j=0; j<grid_boxes[i].right_n; j++){
        //printf("%d ", grid_boxes[i].right_list[j]);
      }
      printf("\ntop neighbours: ");

      for(int j=0; j<grid_boxes[i].top_n; j++){
       // printf("%d ", grid_boxes[i].top_list[j]);
      }
      printf("\nbottom neighbours: ");

      for(int j=0; j<grid_boxes[i].bot_n; j++){
      //  printf("%d ", grid_boxes[i].bot_list[j]);
      }

      printf("\n*******\n");
      
  }
}

void calculateDsvForBox(int box_index){

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
        //cur_box = grid_boxes[cur].top_list[cur][tn];
        cur_box = top_list[cur][tn];
        ov_start = imax(grid_boxes[cur_box].xc, cxc);
        ov_end = imin(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
        overlap = ov_end - ov_start;
        dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
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
        //cur_box = grid_boxes[cur].right_list[rn];
        cur_box = right_list[cur][rn];
        ov_start = imax(grid_boxes[cur_box].yc, cyc);
        ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
        overlap = ov_end - ov_start;
        dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
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
        //cur_box = grid_boxes[cur].bot_list[bn];
        cur_box = bot_list[cur][bn];
        ov_start = imax(grid_boxes[cur_box].xc, cxc);
        ov_end = imin(grid_boxes[cur_box].xc + grid_boxes[cur_box].width, cxc + cw);
        overlap = ov_end - ov_start;
        //printf("bottom Neighbour and temp overlap %d %d %lf\n", overlap, cur_box, grid_boxes[cur_box].temp);
        dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
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
        //cur_box = grid_boxes[cur].left_list[ln];
        cur_box = left_list[cur][ln];
        ov_start = imax(grid_boxes[cur_box].yc, cyc);
        ov_end = imin(grid_boxes[cur_box].yc + grid_boxes[cur_box].height, cyc + ch);
        overlap = ov_end - ov_start;
        dsv_c[cur] += (overlap*grid_boxes[cur_box].temp);
    }
  }
  double offset = 0;

  double cur_temp = grid_boxes[cur].temp;
  if(box_peri > 0){
    double avg_dsv = dsv_c[cur]/(double)box_peri;
    offset = ((cur_temp - avg_dsv)*affect_rate);
  }
  dsv_c[cur] = cur_temp - offset;
}

void compute_dsv(box_range *mydata){
  //printf("computing dsv");
  //thread_range *mydata = (thread_range*) range;
  //printf("computing dsv from %d to %d \n",  mydata->start, mydata->end );
  for(int i = mydata->start; i <= min(mydata->end, total_boxes-1); i++){
    calculateDsvForBox(i);
  }
}

int main(int argc, char *argv[]){
  
  

  MPI_Init(NULL, NULL);

  int p_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  
    
  int blocklengths[10] = {1,1,1,1,1,1,1,1,1,1};
  MPI_Datatype types[10] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT,MPI_INT ,MPI_INT, MPI_INT, MPI_DOUBLE};
  MPI_Aint array_of_displacements[] = { offsetof( Grid_box, box_id ),
                                        offsetof( Grid_box, top_n ),
                                        offsetof( Grid_box, bot_n ),
                                        offsetof( Grid_box, left_n ),
                                        offsetof( Grid_box, right_n ),
                                        offsetof( Grid_box, xc ),
                                        offsetof( Grid_box, yc ),
                                        offsetof( Grid_box, height ),
                                        offsetof( Grid_box, width ),
                                        offsetof( Grid_box, temp )
                                    };
                                      
  MPI_Datatype grid_box_type;
   
    
  MPI_Type_create_struct(10, blocklengths, array_of_displacements, types, &grid_box_type);
  MPI_Type_commit(&grid_box_type);
  
  printf("process rank started %d\n", p_rank);
  
  int total_iterations = 0;
  Grid_box *gb_chunks; 


  
  if(p_rank == 0) {
    
    int row = 0;
    int col = 0;
    char line[500];
    int linecounter = 0;
    char delim[] = " \t";
    int threads_created = 0;
     
    int i=0;
    int j=0;
    int k=0;
      
    struct timespec start, end;
    double timediff;
    printf("runs 0 multiple times\n");

    if(argc != 3){
      printf("Please provide correct number of arguments in the following order: <Number of threads> <AFFECT_RATE> <Number of threads> <EPSILON> <INPUT_FILE>\n");
      exit(0);
    }
    
    sscanf(argv[1], "%lf", &affect_rate);
    sscanf(argv[2], "%lf", &epsilon);
      //reading first line containng number of boxes, rows and cols
    if(fgets(line, sizeof(line), stdin)){
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
    
    int num_divs = total_boxes/NUM_WORKERS;
    printf("total boxes seen %d\n", total_boxes);
    printf("Num divs %d\n", num_divs);
    int init_index = 0;
    //do all the mallocs
    grid_boxes = malloc(sizeof(Grid_box) * total_boxes);

    top_list = malloc(sizeof(int*)*total_boxes);
    bot_list = malloc(sizeof(int*)*total_boxes);
    left_list = malloc(sizeof(int*)*total_boxes);
    right_list = malloc(sizeof(int*)*total_boxes);

    dsv_c = malloc(sizeof(double) * total_boxes);
    
    printf("done with mallocs \n");
    int t=0;
    
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
        while(ptr != NULL){
          
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
            top_list[t] = malloc(sizeof(int)*(gb.top_n));
            printf("done with mallocs to top list \n");
            //gb.top_list = malloc(sizeof(int)*(gb.top_n));
          }else if(ptr && i>=1 && j < gb.top_n){
             top_list[t][j] = (int) strtol(ptr, (char **)NULL, 10);
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
            bot_list[t] = malloc(sizeof(int)*(gb.bot_n));
             //gb.bot_list = malloc(sizeof(int)*(gb.bot_n));
          }else if(ptr && i>=1 &&j < gb.bot_n){
             bot_list[t][j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
        }
      }else if(linecounter == 4){
        j=0;
        i=0;
        while(ptr != NULL){
         if(ptr && i==0){
            gb.left_n = (int) strtol(ptr, (char **)NULL, 10);
            left_list[t] = malloc(sizeof(int)*(gb.left_n));
           // gb.left_list = malloc(sizeof(int)*(gb.left_n));
          }else if(ptr && i>=1  &&j < gb.left_n){
             left_list[t][j] = (int) strtol(ptr, (char **)NULL, 10);
             j++;
          }
          i++;
          ptr = strtok(NULL, delim);
            
        }
      }else if(linecounter == 5){
        j=0;
        i=0;
        while(ptr != NULL){
          if(ptr && i==0){
            gb.right_n = (int) strtol(ptr, (char **)NULL, 10);
            right_list[t] = malloc(sizeof(int)*(gb.right_n));
            //gb.right_list = malloc(sizeof(int)*(gb.right_n));
          }else if(ptr && i>=1  &&j < gb.right_n){
             right_list[t][j] = (int) strtol(ptr, (char **)NULL, 10);
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
      printf("done with all mallocs for box %d\n", t);
      printf("line counter %d\n", linecounter);
      if(t==total_boxes)break;
    }
    
    printf("good till here parsed file in %d process \n", p_rank);
   
    //sending data to other process 
    printf("broadcast total_boxes to all other processes \n");
    MPI_Bcast(&total_boxes, 1, MPI_INT, 0, MPI_COMM_WORLD);
            
       //divide the grids among threads
    for (int i = 0; i < NUM_WORKERS; i++) {
  // MPI_Send(&total_boxes, 1, MPI_INT, i+1, 12, MPI_COMM_WORLD);
     int off_index = num_divs*i;
     if(i < NUM_WORKERS-1)
        MPI_Send(&grid_boxes[off_index], num_divs, grid_box_type, i+1, 14, MPI_COMM_WORLD);
     else
        MPI_Send(&grid_boxes[off_index], num_divs + (total_boxes%4), grid_box_type, i+1, 14, MPI_COMM_WORLD);
    
    }
      
    
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  if(p_rank > 0){
    //receive data
     MPI_Status status;
     const int src=0;
     int total_boxes = 0;
     printf("receiving at process %d\n", p_rank);
     MPI_Bcast(&total_boxes, 1, MPI_INT, 0, MPI_COMM_WORLD);
      //MPI_Recv(&total_boxes, 1, MPI_INT, src, 12, MPI_COMM_WORLD, &status);
      printf("total boxes received at %d, is %d \n", p_rank, total_boxes);
      int num_divs = total_boxes/NUM_WORKERS;
      printf("num_divs received %d\n", num_divs);
    
    Grid_box *gb_recv;
    
    
     if(p_rank < NUM_WORKERS){
          printf("allocating p_rank %d %d\n", p_rank, num_divs);
         gb_recv =  malloc(sizeof(Grid_box) * num_divs);
        MPI_Recv(&gb_recv, num_divs, grid_box_type, src, 14, MPI_COMM_WORLD, &status);
     }else{
          printf("allocating %d %d\n", p_rank, (total_boxes - (NUM_WORKERS-1)*num_divs ) );
         gb_recv =  malloc(sizeof(Grid_box) * (total_boxes - (NUM_WORKERS-1)*num_divs ) );
        MPI_Recv(&gb_recv, (total_boxes - (NUM_WORKERS-1)*num_divs ), grid_box_type, src, 14, MPI_COMM_WORLD, &status);
     }
        

    
    printf("Rank %d: Received: box_id = %d height = %d\n", p_rank,
             gb_recv[0].box_id, gb_recv[0].height);
  }
  
  MPI_Finalize();
  return 0;
}


