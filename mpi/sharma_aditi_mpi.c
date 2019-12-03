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
int p_rank;
       Grid_box *gb_recv;
     int **rbot_list;
     int **rtop_list;
     int **rleft_list;
     int **rright_list;
     int num_divs, total_boxes;
     double *dsv_c;
        
    struct timespec start, end;
    struct timeval t_start, t_end, t_diff;
  

    clock_t start_clock, end_clock;


    time_t time_t_start;
    int total_iterations = 0;


void printBoxes(Grid_box *grid_boxes, int **top_list, int **bot_list, int **left_list, int **right_list, int total_boxes){
   // printf("p_rank in printbox %d \n", p_rank);
    for(int i=0; i<total_boxes; i++){
      printf("box id %d\n", grid_boxes[i].box_id);
      printf("box temprature %lf\n", grid_boxes[i].temp);
      printf("box x %d\n", grid_boxes[i].xc);
      printf("box y %d\n", grid_boxes[i].yc);
      printf("box height and width %d    %d\n", grid_boxes[i].height, grid_boxes[i].width);
      
      printf("Left neighbours: ");
      for(int j=0; j<grid_boxes[i].left_n; j++){
        printf("%d ", left_list[i][j]);
      }
      printf("\nRight neighbours: ");

      for(int j=0; j<grid_boxes[i].right_n; j++){
        printf("%d ", right_list[i][j]);
      }
      printf("\ntop neighbours: ");

      for(int j=0; j<grid_boxes[i].top_n; j++){
       printf("%d ", top_list[i][j]);
      }
      printf("\nbottom neighbours: ");

      for(int j=0; j<grid_boxes[i].bot_n; j++){
        printf("%d ", bot_list[i][j]);
      }

      printf("\n*******\n");
      
  }
}

void calculateDsvForBox(Grid_box *grid_boxes, int **top_list, int **bot_list, int **left_list, int **right_list, int box_index, double *dsv_c){
  
 // printf("box id is %d---- p_rank %d  temp[%d]:  %lf\n", box_index, p_rank, box_index, grid_boxes[box_index].temp);
  
  
  int cur = box_index;
  int cxc = grid_boxes[cur].xc;
  int cyc = grid_boxes[cur].yc;
  int ch = grid_boxes[cur].height;
  int cw = grid_boxes[cur].width;
  dsv_c[cur] = 0;
  int box_peri = 0;
  double acc = 0;
  
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
        acc += (overlap*grid_boxes[cur_box].temp);
    }
  //  printf("top dsv_c[%d] :%lf  %lf \n",cur, dsv_c[cur], acc  );
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
        acc += (overlap*grid_boxes[cur_box].temp);
    }
   // printf("right dsv_c[%d] :%lf  %lf \n",cur, dsv_c[cur], acc  );
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
        acc += (overlap*grid_boxes[cur_box].temp);
    }
   // printf("bot dsv_c[%d] :%lf  %lf \n",cur, dsv_c[cur], acc  );
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
        acc += (overlap*grid_boxes[cur_box].temp);
    }
   // printf("left dsv_c[%d] :%lf  %lf \n",cur, dsv_c[cur], acc );
  }
  double offset = 0;

  double cur_temp = grid_boxes[cur].temp;
 //printf("box peri[i] : %d %d \n",cur, box_peri );
  if(box_peri > 0){
    double avg_dsv = dsv_c[cur]/(double)box_peri;
    offset = ((cur_temp - avg_dsv)*affect_rate);
  }
  dsv_c[cur] = cur_temp - offset;
}


int main(int argc, char *argv[]){
  
  

  MPI_Init(NULL, NULL);

  
  MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm slaves;
  MPI_Comm_split( MPI_COMM_WORLD, ( p_rank == 0 ), p_rank, &slaves );
  
    
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
  
  Grid_box *gb_chunks; 
  
  if(p_rank == 0) {
    
    int row = 0;
    int col = 0;
    char *line;
    int linecounter = 0;
    char delim[] = " \t";
    int threads_created = 0;
     
    int i=0;
    int j=0;
    int k=0;
      
    struct timespec start, end;
    double timediff;
    //printf("runs 0 multiple times\n");

    // if(argc != 3){
    //   printf("Please provide correct number of arguments in the following order: <Number of threads> <AFFECT_RATE> <Number of threads> <EPSILON> <INPUT_FILE>\n");
    //   exit(0);
    // }
    
    // sscanf(argv[1], "%lf", &affect_rate);
    // sscanf(argv[2], "%lf", &epsilon);
      //reading first line containng number of boxes, rows and cols
    FILE * fp;
    fp = fopen("testgrid_400_12206", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
     
     size_t len = 0;
     ssize_t read;
        
    if((read = getline(&line, &len, fp)) != -1){
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
  //  printf("total boxes seen %d\n", total_boxes);
    //printf("Num divs %d\n", num_divs);
    int init_index = 0;
    //do all the mallocs
    grid_boxes = malloc(sizeof(Grid_box) * total_boxes);

    top_list = malloc(sizeof(int*)*total_boxes);
    bot_list = malloc(sizeof(int*)*total_boxes);
    left_list = malloc(sizeof(int*)*total_boxes);
    right_list = malloc(sizeof(int*)*total_boxes);

    dsv_c = malloc(sizeof(double) * total_boxes);
    
   // printf("done with mallocs \n");
    int t=0;
    
    while((read = getline(&line, &len, fp)) != -1) {
        
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
         //   printf("done with mallocs to top list \n");
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

      if(t==total_boxes)break;
    }
    

   
    MPI_Send(&epsilon, 1, MPI_DOUBLE, 1,10, MPI_COMM_WORLD);
    MPI_Send(&affect_rate, 1, MPI_DOUBLE, 1,11, MPI_COMM_WORLD);
    MPI_Send(&total_boxes, 1, MPI_INT, 1,12, MPI_COMM_WORLD);

    
  
 
  }

   MPI_Status status;
 
  //printf("sent all boxes \n");
  if(p_rank == 0){
        MPI_Send(&grid_boxes[0], total_boxes, grid_box_type, 1, 14, MPI_COMM_WORLD);
  }
  if(p_rank == 1){
     
      int rt = 0;
       MPI_Recv(&epsilon, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(&affect_rate, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, &status);
       MPI_Recv(&total_boxes, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
        gb_recv = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
     //  printf("total boxes  in 1, %d\n", rt);
      MPI_Recv(&gb_recv[0], total_boxes, grid_box_type, 0, 14, MPI_COMM_WORLD, &status);
      
       
  }
   // MPI_Barrier(MPI_COMM_WORLD);
  if(p_rank == 0){
         
    for(int p=1; p<=1; p++){
        for(int i=0; i<total_boxes; i++){
            MPI_Send(&top_list[i][0], grid_boxes[i].top_n, MPI_INT, p, 15, MPI_COMM_WORLD);
            
        }
    }
  }
  
   if(p_rank == 1){
        rtop_list =  malloc(sizeof(int*)*total_boxes);
      

         for(int i=0; i<total_boxes; i++){

                 rtop_list[i] = malloc(sizeof(int) * gb_recv[i].top_n);
                MPI_Recv(&rtop_list[i][0], gb_recv[i].top_n, MPI_INT, 0, 15, MPI_COMM_WORLD, &status);
     
        }
      //  printf("Awesome got all top neighbores \n");
   }
   
     if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&bot_list[i][0], grid_boxes[i].bot_n, MPI_INT, p, 16, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 1){
     
        rbot_list =  malloc(sizeof(int*)*total_boxes);

         for(int i=0; i<total_boxes; i++){

                 rbot_list[i] = malloc(sizeof(int) * gb_recv[i].bot_n);
                MPI_Recv(&rbot_list[i][0], gb_recv[i].bot_n, MPI_INT, 0, 16, MPI_COMM_WORLD, &status);
     
        }
      //  printf("Awesome got all bottom neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&right_list[i][0], grid_boxes[i].right_n, MPI_INT,  p, 17, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 1){


      rright_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rright_list[i] = malloc(sizeof(int) * gb_recv[i].right_n);
              MPI_Recv(&rright_list[i][0], gb_recv[i].right_n, MPI_INT, 0, 17, MPI_COMM_WORLD, &status);
   
      }
   //   printf("Awesome got all right neighbores \n");
   }
   
       if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
               MPI_Send(&left_list[i][0], grid_boxes[i].left_n, MPI_INT,  p, 18, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 1){


      rleft_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rleft_list[i] = malloc(sizeof(int) * gb_recv[i].left_n);
              MPI_Recv(&rleft_list[i][0], gb_recv[i].left_n, MPI_INT, 0, 18, MPI_COMM_WORLD, &status);
   
      }
    //  printf("Awesome got all left neighbores \n");
   }
   
   MPI_Barrier(MPI_COMM_WORLD);
   
   if(p_rank == 0){
       MPI_Send(&epsilon, 1, MPI_DOUBLE, 2,10, MPI_COMM_WORLD);
        MPI_Send(&affect_rate, 1, MPI_DOUBLE, 2,11, MPI_COMM_WORLD);
        MPI_Send(&total_boxes, 1, MPI_INT, 2,12, MPI_COMM_WORLD);
        MPI_Send(&grid_boxes[0], total_boxes, grid_box_type, 2, 14, MPI_COMM_WORLD);
  }
  if(p_rank == 2){
     
      int rt = 0;
       MPI_Recv(&epsilon, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(&affect_rate, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, &status);
       MPI_Recv(&total_boxes, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
        gb_recv = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
     //  printf("total boxes  in 1, %d\n", total_boxes);
      MPI_Recv(&gb_recv[0], total_boxes, grid_box_type, 0, 14, MPI_COMM_WORLD, &status);
      
       
  }
   // MPI_Barrier(MPI_COMM_WORLD);
  if(p_rank == 0){
         
    for(int p=1; p<=1; p++){
        for(int i=0; i<total_boxes; i++){
            MPI_Send(&top_list[i][0], grid_boxes[i].top_n, MPI_INT, 2, 15, MPI_COMM_WORLD);
            
        }
    }
  }
  
   if(p_rank == 2){
        rtop_list =  malloc(sizeof(int*)*total_boxes);
      

         for(int i=0; i<total_boxes; i++){

             rtop_list[i] = malloc(sizeof(int) * gb_recv[i].top_n);
            MPI_Recv(&rtop_list[i][0], gb_recv[i].top_n, MPI_INT, 0, 15, MPI_COMM_WORLD, &status);
     
        }
       // printf("Awesome got all top neighbores \n");
   }
   
     if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&bot_list[i][0], grid_boxes[i].bot_n, MPI_INT, 2, 16, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 2){
     
        rbot_list =  malloc(sizeof(int*)*total_boxes);

         for(int i=0; i<total_boxes; i++){

                 rbot_list[i] = malloc(sizeof(int) * gb_recv[i].bot_n);
                MPI_Recv(&rbot_list[i][0], gb_recv[i].bot_n, MPI_INT, 0, 16, MPI_COMM_WORLD, &status);
     
        }
      //  printf("Awesome got all bottom neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&right_list[i][0], grid_boxes[i].right_n, MPI_INT,  2, 17, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 2){


      rright_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rright_list[i] = malloc(sizeof(int) * gb_recv[i].right_n);
              MPI_Recv(&rright_list[i][0], gb_recv[i].right_n, MPI_INT, 0, 17, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all right neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
               MPI_Send(&left_list[i][0], grid_boxes[i].left_n, MPI_INT,  2, 18, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 2){


      rleft_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rleft_list[i] = malloc(sizeof(int) * gb_recv[i].left_n);
              MPI_Recv(&rleft_list[i][0], gb_recv[i].left_n, MPI_INT, 0, 18, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all left neighbores \n");
   }
   

  
  MPI_Barrier(MPI_COMM_WORLD);
  
     if(p_rank == 0){
       MPI_Send(&epsilon, 1, MPI_DOUBLE, 3,10, MPI_COMM_WORLD);
        MPI_Send(&affect_rate, 1, MPI_DOUBLE, 3,11, MPI_COMM_WORLD);
        MPI_Send(&total_boxes, 1, MPI_INT, 3,12, MPI_COMM_WORLD);
        MPI_Send(&grid_boxes[0], total_boxes, grid_box_type, 3, 14, MPI_COMM_WORLD);
  }
  if(p_rank == 3){
     
      int rt = 0;
       MPI_Recv(&epsilon, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(&affect_rate, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, &status);
       MPI_Recv(&total_boxes, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
        gb_recv = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
      // printf("total boxes  in 1, %d\n", total_boxes);
      MPI_Recv(&gb_recv[0], total_boxes, grid_box_type, 0, 14, MPI_COMM_WORLD, &status);
      
       
  }
   // MPI_Barrier(MPI_COMM_WORLD);
  if(p_rank == 0){
         
    for(int p=1; p<=1; p++){
        for(int i=0; i<total_boxes; i++){
            MPI_Send(&top_list[i][0], grid_boxes[i].top_n, MPI_INT, 3, 15, MPI_COMM_WORLD);
            
        }
    }
  }
  
   if(p_rank == 3){
        rtop_list =  malloc(sizeof(int*)*total_boxes);
      

         for(int i=0; i<total_boxes; i++){

             rtop_list[i] = malloc(sizeof(int) * gb_recv[i].top_n);
            MPI_Recv(&rtop_list[i][0], gb_recv[i].top_n, MPI_INT, 0, 15, MPI_COMM_WORLD, &status);
     
        }
      //  printf("Awesome got all top neighbores \n");
   }
   
     if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&bot_list[i][0], grid_boxes[i].bot_n, MPI_INT, 3, 16, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 3){
     
        rbot_list =  malloc(sizeof(int*)*total_boxes);

         for(int i=0; i<total_boxes; i++){

                 rbot_list[i] = malloc(sizeof(int) * gb_recv[i].bot_n);
                MPI_Recv(&rbot_list[i][0], gb_recv[i].bot_n, MPI_INT, 0, 16, MPI_COMM_WORLD, &status);
     
        }
      //  printf("Awesome got all bottom neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&right_list[i][0], grid_boxes[i].right_n, MPI_INT,  3, 17, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 3){


      rright_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rright_list[i] = malloc(sizeof(int) * gb_recv[i].right_n);
              MPI_Recv(&rright_list[i][0], gb_recv[i].right_n, MPI_INT, 0, 17, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all right neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
               MPI_Send(&left_list[i][0], grid_boxes[i].left_n, MPI_INT,  3, 18, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 3){


      rleft_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rleft_list[i] = malloc(sizeof(int) * gb_recv[i].left_n);
              MPI_Recv(&rleft_list[i][0], gb_recv[i].left_n, MPI_INT, 0, 18, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all left neighbores \n");
   }
   
   
   MPI_Barrier(MPI_COMM_WORLD);
   
   
        if(p_rank == 0){
       MPI_Send(&epsilon, 1, MPI_DOUBLE, 4,10, MPI_COMM_WORLD);
        MPI_Send(&affect_rate, 1, MPI_DOUBLE, 4,11, MPI_COMM_WORLD);
        MPI_Send(&total_boxes, 1, MPI_INT, 4,12, MPI_COMM_WORLD);
        MPI_Send(&grid_boxes[0], total_boxes, grid_box_type, 4, 14, MPI_COMM_WORLD);
  }
  if(p_rank == 4){
     
      int rt = 0;
       MPI_Recv(&epsilon, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(&affect_rate, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, &status);
       MPI_Recv(&total_boxes, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
        gb_recv = (Grid_box*)malloc(sizeof(Grid_box) * total_boxes);
     //  printf("total boxes  in 1, %d\n", total_boxes);
      MPI_Recv(&gb_recv[0], total_boxes, grid_box_type, 0, 14, MPI_COMM_WORLD, &status);
      
       
  }
   // MPI_Barrier(MPI_COMM_WORLD);
  if(p_rank == 0){
         
    for(int p=1; p<=1; p++){
        for(int i=0; i<total_boxes; i++){
            MPI_Send(&top_list[i][0], grid_boxes[i].top_n, MPI_INT, 4, 15, MPI_COMM_WORLD);
            
        }
    }
  }
  
   if(p_rank == 4){
        rtop_list =  malloc(sizeof(int*)*total_boxes);
      

         for(int i=0; i<total_boxes; i++){

             rtop_list[i] = malloc(sizeof(int) * gb_recv[i].top_n);
            MPI_Recv(&rtop_list[i][0], gb_recv[i].top_n, MPI_INT, 0, 15, MPI_COMM_WORLD, &status);
     
        }
        //printf("Awesome got all top neighbores \n");
   }
   
     if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&bot_list[i][0], grid_boxes[i].bot_n, MPI_INT, 4, 16, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 4){
     
        rbot_list =  malloc(sizeof(int*)*total_boxes);

         for(int i=0; i<total_boxes; i++){

                 rbot_list[i] = malloc(sizeof(int) * gb_recv[i].bot_n);
                MPI_Recv(&rbot_list[i][0], gb_recv[i].bot_n, MPI_INT, 0, 16, MPI_COMM_WORLD, &status);
     
        }
        //printf("Awesome got all bottom neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
                MPI_Send(&right_list[i][0], grid_boxes[i].right_n, MPI_INT,  4, 17, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 4){


      rright_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rright_list[i] = malloc(sizeof(int) * gb_recv[i].right_n);
              MPI_Recv(&rright_list[i][0], gb_recv[i].right_n, MPI_INT, 0, 17, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all right neighbores \n");
   }
   
    if(p_rank == 0){
         
        for(int p=1; p<=1; p++){
            for(int i=0; i<total_boxes; i++){
               MPI_Send(&left_list[i][0], grid_boxes[i].left_n, MPI_INT,  4, 18, MPI_COMM_WORLD);
                
            }
        }
    }
  
   if(p_rank == 4){


      rleft_list =  malloc(sizeof(int*)*total_boxes);


       for(int i=0; i<total_boxes; i++){

               rleft_list[i] = malloc(sizeof(int) * gb_recv[i].left_n);
              MPI_Recv(&rleft_list[i][0], gb_recv[i].left_n, MPI_INT, 0, 18, MPI_COMM_WORLD, &status);
   
      }
     // printf("Awesome got all left neighbores \n");
   }
   
   
   MPI_Barrier(MPI_COMM_WORLD);
   

  if(p_rank > 0){
    //receive data
     MPI_Status status;
     const int src=0;

    num_divs = total_boxes/NUM_WORKERS;
  //  printf("num_divs received %d\n", num_divs);
    dsv_c = (double*) malloc(total_boxes * sizeof(double));

  }
  
  double *up_dsv = malloc(sizeof(double) * total_boxes);
  int tt=0;
  int diff = 0;
  
  int run = 1;
  double *dsv_c1 = malloc(sizeof(double) * total_boxes);
  double *dsv_c2 = malloc(sizeof(double) * total_boxes);
  double *dsv_c3 = malloc(sizeof(double) * total_boxes);
  double *dsv_c4 = malloc(sizeof(double) * total_boxes);
  
  if(p_rank == 0){
       time_t_start = time(NULL); 
       start_clock = clock();
       clock_gettime(CLOCK_REALTIME,& start);
  }
  
  
   MPI_Barrier(MPI_COMM_WORLD);
  while(1){
      tt++;
     if(p_rank == 1){
        // printf("calculating dsv\n");
         
          int num_threads_req = 28;
          int threads_created = 0;
          int num_divs = total_boxes/(2*num_threads_req);
          thread_range t_message[num_threads_req];
          int init_index = 0;
        
          //divide the grids among threads
          for (int i = 0; i < num_threads_req; i++) {
            t_message[i].thread_id = i;
            t_message[i].start = init_index;
            if (i == (num_threads_req - 1)) {
              t_message[i].end = total_boxes/2 - 1;
            } else {
              t_message[i].end = (init_index + num_divs - 1);
            }
            init_index = init_index + num_divs;
          }

          #pragma omp parallel num_threads(num_threads_req)
          {
              int thread_id = omp_get_thread_num();
              if (thread_id == 0) {
                  threads_created = omp_get_num_threads();
                }
                
                for(int i = t_message[thread_id].start; i <= min(t_message[thread_id].end, (total_boxes/2 - 1)); i++){
                    calculateDsvForBox(gb_recv, rtop_list, rbot_list, rleft_list, rright_list, i, dsv_c1);
                }

                #pragma omp barrier
           }

         MPI_Send(dsv_c1, 12206/2, MPI_DOUBLE, 0,22, MPI_COMM_WORLD);
     }
     
     if(p_rank == 0){
         MPI_Recv(up_dsv, 12206/2, MPI_DOUBLE, 1, 22, MPI_COMM_WORLD, &status);
     }
      
     // MPI_Barrier(MPI_COMM_WORLD);
     
      if(p_rank == 2){
        // printf("calculating dsv\n");
          int num_threads_req = 28;
          int threads_created = 0;
          int num_divs = total_boxes/(2*num_threads_req);
          thread_range t_message[num_threads_req];
          int init_index = total_boxes/2;
        
          //divide the grids among threads
          for (int i = 0; i < num_threads_req; i++) {
            t_message[i].thread_id = i;
            t_message[i].start = init_index;
            if (i == (num_threads_req - 1)) {
              t_message[i].end = total_boxes;
            } else {
              t_message[i].end = (init_index + num_divs - 1);
            }
            init_index = init_index + num_divs;
          }

          #pragma omp parallel num_threads(num_threads_req)
          {
              int thread_id = omp_get_thread_num();
              if (thread_id == 0) {
                  threads_created = omp_get_num_threads();
                }
                
                for(int i = t_message[thread_id].start; i <= min(t_message[thread_id].end, (total_boxes-1)); i++){
                    calculateDsvForBox(gb_recv, rtop_list, rbot_list, rleft_list, rright_list, i, dsv_c2);
                }

                #pragma omp barrier
           }

         MPI_Send(&dsv_c2[12206/2], 12206/2, MPI_DOUBLE, 0,22, MPI_COMM_WORLD);
     }
     
     if(p_rank == 0){
         MPI_Recv(&up_dsv[12206/2], 12206/2, MPI_DOUBLE, 2, 22, MPI_COMM_WORLD, &status);
     }

     if(p_rank == 0){
        // MPI_Recv(up_dsv, 12206, MPI_DOUBLE, 1, 22, MPI_COMM_WORLD, &status);
          cur_min_dsv =  up_dsv[0];
          cur_max_dsv =  up_dsv[0];
          grid_boxes[0].temp = up_dsv[0];
         for(int curx=1; curx<12206; curx++){
                grid_boxes[curx].temp = up_dsv[curx];
                cur_max_dsv = max(cur_max_dsv,  up_dsv[curx]);
                cur_min_dsv = min(cur_min_dsv,  up_dsv[curx]);
         }
           // tt++;
          
        // printf("cur max: %lf  cur min: %lf \n", cur_max_dsv, cur_min_dsv );
          diff = (cur_max_dsv - cur_min_dsv) <= (epsilon*cur_max_dsv) ? 1 : 0;
          if(diff==1){
              run =0;
              
          }
          
           MPI_Send(up_dsv, 12206, MPI_DOUBLE, 1,23, MPI_COMM_WORLD);
          MPI_Send(up_dsv, 12206, MPI_DOUBLE, 2,23, MPI_COMM_WORLD);
          //  MPI_Send(up_dsv, 12206, MPI_DOUBLE, 3,23, MPI_COMM_WORLD);
            
         MPI_Send(&run, 1, MPI_INT, 1,24, MPI_COMM_WORLD);
         MPI_Send(&run, 1, MPI_INT, 2,24, MPI_COMM_WORLD);
         MPI_Send(&run, 1, MPI_INT, 3,24, MPI_COMM_WORLD);
         MPI_Send(&run, 1, MPI_INT, 4,24, MPI_COMM_WORLD);
     }

     if(p_rank == 1){
         MPI_Recv(dsv_c1, 12206, MPI_DOUBLE, 0, 23, MPI_COMM_WORLD, &status);
          MPI_Recv(&run, 1, MPI_INT, 0, 24, MPI_COMM_WORLD, &status);
          
          if(run == 0){
            //  printf("p 1 breaking\n");
              break;
          }
         for(int i=0; i<total_boxes; i++){
             gb_recv[i].temp = dsv_c1[i];
             dsv_c1[i] = 0;
         }
     }
     
     if(p_rank == 2){
          MPI_Recv(dsv_c2, 12206, MPI_DOUBLE, 0, 23, MPI_COMM_WORLD, &status);
          MPI_Recv(&run, 1, MPI_INT, 0, 24, MPI_COMM_WORLD, &status);
          
          if(run == 0){
            //  printf("p 2 breaking\n");
              break;
          }
          for(int i=0; i<total_boxes; i++){
             gb_recv[i].temp = dsv_c2[i];
             dsv_c2[i] = 0;
         }
       
     }
     
     if(p_rank == 3){
       
          MPI_Recv(&run, 1, MPI_INT, 0, 24, MPI_COMM_WORLD, &status);
          
          if(run == 0){
             // printf("p 2 breaking\n");
              break;
          }
       
     }
     
     if(p_rank == 4){
       
          MPI_Recv(&run, 1, MPI_INT, 0, 24, MPI_COMM_WORLD, &status);
          
          if(run == 0){
           //   printf("p 2 breaking\n");
              break;
          }
       
     }
     
   
     if(run == 0){
         if(p_rank == 0){
           //  printf("cur max: %lf  cur min: %lf \n", cur_max_dsv, cur_min_dsv );
         }
        // printf("process %d\n", p_rank);
         break;
     }
     
   // MPI_Barrier(MPI_COMM_WORLD);
 }
     
  
  
  if(p_rank == 0){

         
    time_t time_t_end;
    time_t_end = time(NULL); 

    double elapsed=0;
    clock_gettime(CLOCK_REALTIME,&end);
    end_clock = (double)((clock() - start_clock));
    
    double timediff = (double)((end.tv_sec - start.tv_sec)*CLOCKS_PER_SEC + ((end.tv_nsec -start.tv_nsec)/1000000));
    
    
    printf("\n********************************************************************************\n");
    printf("dissipation converged in %d iterations,\n", tt);
    printf("\twith max DSV = %lf and min DSV = %lf\n", cur_max_dsv, cur_min_dsv);

    printf("\taffect rate = %lf; epsilon = %lf\n\n", affect_rate, epsilon);
    printf("elapsed convergence loop time (clock_gettime()): %lf\n", timediff);
    printf("elapsed convergence loop time (clock): %ld\n", end_clock);
    printf("elapsed convergence loop time (time_t): %ld\n", (time_t_end - time_t_start));
    printf("\n********************************************************************************\n");
  }
  
  
  MPI_Finalize();
  return 0;
}

