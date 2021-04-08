#include <assert.h>

#include <algorithm>

#include "sudoku.h"

static int arity(int cell,int *board)//修改参数
{
  bool occupied[10] = {false};
  for (int i = 0; i < NEIGHBOR; ++i) {
    int neighbor = neighbors[cell][i];
    occupied[board[neighbor]] = true;
  }
  return std::count(occupied+1, occupied+10, false);
}

static void find_min_arity(int space,int *spaces,int *board)//修改参数 传递指针
{
  int cell = spaces[space];
  int min_space = space;
  int min_arity = arity(cell,board);

  for (int sp = space+1; sp < nspaces && min_arity > 1; ++sp) {
    int cur_arity = arity(spaces[sp],board);
    if (cur_arity < min_arity) {
      min_arity = cur_arity;
      min_space = sp;
    }
  }

  if (space != min_space) {
    std::swap(spaces[min_space], spaces[space]);
  }
  //return spaces;
}

bool solve_sudoku_min_arity(int which_space,int *board,int nspaces,int *spaces)//修改参数
{
  if (which_space >= nspaces) {
    return true;
  }

  find_min_arity(which_space,spaces,board);

  int cell = spaces[which_space];

  for (int guess = 1; guess <= NUM; ++guess) {
    if (available(guess,cell,board)) {
      // hold
      assert(board[cell] == 0);
      board[cell] = guess;

      // try
      if (solve_sudoku_min_arity(which_space+1,board,nspaces,spaces)) {//修改递归参数
        return true;
      }

      // unhold
      assert(board[cell] == guess);
      board[cell] = 0;
    }
  }
  return false;
}
