#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "sudoku.h"
#define PTHREAD_NUM 4 //线程数目为4
char puzzle[tasks][128];//存储数独
int total = 0;
int fin=0;//当前数独的序号
bool finish=false;//是否解决

pthread_t pt[PTHREAD_NUM];//线程号
pthread_mutex_t mutex;//🔒

int getWorkID()
{
    int workID=0;
    pthread_mutex_lock(&mutex);
    if (fin>=total){
        pthread_mutex_unlock(&mutex);
        return -1;
    }//当所有任务已经解决则返回-1
    workID=fin++;
    if (workID==total-1){//当前已经没有可分配的任务
        finish=true;
    }
    pthread_mutex_unlock(&mutex);
    return workID;//返回分配的ID
}

void *Run(void *args)
{

}

void Create()//创建线程
{
    for (int i = 0; i < PTHREAD_NUM; i++){
        if (pthread_create(&pt[i], NULL, Run, NULL)!= 0){
            perror("error: pthread create failed");
            exit(1);
        }
    }
}

int64_t now()//⏲
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");
  bool (*solve)(int) = solve_sudoku_basic;
  if (argv[2] != NULL)
    if (argv[2][0] == 'a')
      solve = solve_sudoku_min_arity;
    else if (argv[2][0] == 'c')
      solve = solve_sudoku_min_arity_cache;
    else if (argv[2][0] == 'd')
      solve = solve_sudoku_dancing_links;
  int64_t start = now();//⏲

  Create();
  /*while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
    if (strlen(puzzle) >= N) {
      ++total;
      input(puzzle);
      init_cache();
      //if (solve_sudoku_min_arity_cache(0)) {
      //if (solve_sudoku_min_arity(0))
      //if (solve_sudoku_basic(0)) {
      if (solve(0)) {
        ++total_solved;
        if (!solved())
          assert(0);
      }
      else {
        printf("No: %s", puzzle);
      }
    }
  }*/
  int64_t end = now();
  double sec = (end-start)/1000000.0;
  printf("时间:\n");
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total);

  return 0;
}
