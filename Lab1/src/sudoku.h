#ifndef SUDOKU_H
#define SUDOKU_H

const bool DEBUG_MODE = false;
enum { ROW=9, COL=9, N = 81, NEIGHBOR = 20 };
const int NUM = 9;
const int tasks = 5000000;//任务最大数

extern int neighbors[N][NEIGHBOR];
//extern int board[N];
//extern int spaces[N];
extern int nspaces;
extern int (*chess)[COL];

void init_neighbors();
int input(const char in[N], int board[N], int spaces[N]);
void init_cache();

bool available(int guess, int cell,int board[N]);

bool solve_sudoku_basic(int which_space,int board[N],int nspaces,int spaces[N]);
bool solve_sudoku_min_arity(int which_space,int *board,int nspaces,int *spaces);
bool solve_sudoku_min_arity_cache(int which_space);
bool solve_sudoku_dancing_links(int unused);


bool solved(int board[N]);
#endif
