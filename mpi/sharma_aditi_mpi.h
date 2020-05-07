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
  double temp;
} Grid_box;

int **top_list;
int **bot_list;
int **left_list;
int **right_list;

/* Structure sent as param to thread safe func */
typedef struct Thread_range{
  int thread_id; //thread ID.
  int start; //start grid index
  int end; // end grid index
} thread_range;

#define DBL_MAX 1.7976931348623158e+308 /* max value */
#define DBL_MIN 2.2250738585072014e-308 /* min positive value */

#define NUM_WORKERS 4

int total_boxes = 0;
Grid_box *grid_boxes;

<<<<<<< HEAD
double epsilon = 0.1;
double affect_rate = 0.1;
=======
double epsilon = 0.002;
double affect_rate = 0.08;
>>>>>>> ee5ece94bd4d850ae195a439ca9568764e841d0c
int world_size = 5;
int num_threads_req = 1;
int num_process = 5;
double cur_min_dsv = DBL_MAX;
double cur_max_dsv = DBL_MIN;

double min(double a, double b);
double max(double a, double b);
int emptyline(char *line);
int imax(int a, int b);
int imin(int a, int b);

void printBoxes();

<<<<<<< HEAD
void calculateDsvForBox(Grid_box *grid_boxes, int **top_list, int **bot_list, int **left_list, int **right_list, int box_index);
=======
void calculateDsvForBox(Grid_box *grid_boxes, int **top_list, int **bot_list, int **left_list, int **right_list, int box_index, double *dsv_c);
>>>>>>> ee5ece94bd4d850ae195a439ca9568764e841d0c



