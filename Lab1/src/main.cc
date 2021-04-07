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
#define PTHREAD_NUM 5 //线程数目为4
char puzzle[tasks][128];//存储数独
char answer[tasks][128];//存储答案
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
    bool (*solve)(int,int[],int,int[])=solve_sudoku_basic;//使用basic算法
    int board[N];
    int spaces[N];
    while (!finish)
    {
        int id=getWorkID();
        if (id==-1)
            break;
        int t=input(puzzle[id],board,spaces);
        if (solve(0,board,t,spaces))
        {
            if (!solved(board))
                assert(0);
            for (int i=0;i<N;i++)//保存结果
                answer[id][i] = char('0' + board[i]);
        }
        else
        {
            printf("No: %d,无解\n", id);
        }
    }
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

void End()
{
    for (int i = 0; i < PTHREAD_NUM; i++)
        pthread_join(pt[i], NULL);//pthread_join用来等待一个线程的结束
}

int64_t now()//⏲
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void display()
{
    FILE* fw = fopen("answer.txt","w");
    for(int i=0;i<total; i++)
        fprintf(fw,"%s\n",answer[i]);
}

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");
  bool (*solve)(int,int[],int,int[])=solve_sudoku_basic;//使用basic算法
  /*if (argv[2] != NULL)
    if (argv[2][0] == 'a')
      solve = solve_sudoku_min_arity;
    else if (argv[2][0] == 'c')
      solve = solve_sudoku_min_arity_cache;
    else if (argv[2][0] == 'd')
      solve = solve_sudoku_dancing_links; */
  int64_t start = now();//⏲

  Create();
  while (fgets(puzzle[total], sizeof puzzle, fp) != NULL)
    {
        if (strlen(puzzle[total]) >= N)
        {
            pthread_mutex_lock(&mutex);
            total++;
            pthread_mutex_unlock(&mutex);
        }
    }
  End();//结束线程
  //输出答案
  display();
  int64_t end = now();
  double sec = (end-start)/1000000.0;
  printf("时间:\n");
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total);

  return 0;
}
