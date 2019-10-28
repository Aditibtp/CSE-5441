#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/time.h>

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

/* Structure contains the thread specific data. */
typedef struct thread_data{
  int thread_id; // Thread ID.
  int start; // Starting box number in the grid. 
  int end; // End box number in the grid. 
} thread_data;

#define DBL_MAX 1.7976931348623158e+308 /* max value */
#define DBL_MIN 2.2250738585072014e-308 /* min positive value */

int total_boxes = 0;
Grid_box *grid_boxes;
double *dsv_c = NULL;
double epsilon = 0.1;
double affect_rate = 0.1;
int num_threads = 4;
double cur_min_dsv = DBL_MAX;
double cur_max_dsv = DBL_MIN;

double min(double a, double b);
double max(double a, double b);
int emptyline(char *line);
int imax(int a, int b);
int imin(int a, int b);

void printBoxes();

void calculateDsvForBox(int box_index);

void *compute_dsv(void *range);
