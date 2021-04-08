CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku_solve

sudoku_solve: main.cc neighbor.cc sudoku_basic.cc sudoku_min_arity.cc 
	g++ -O2 -o $@ $^ -lpthread
clean:
	-rm  sudoku_solve
