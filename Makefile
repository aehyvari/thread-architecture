all:
	c++ -c -o SMTSolver.o -g -O0 -std=c++17 SMTSolver.cc
	c++ -c -o main.o -g -O0 -std=c++17 main.cc
	c++ -o main main.o SMTSolver.o

clean:
	rm -rf main.o SMTSolver.o main main.dSYM
