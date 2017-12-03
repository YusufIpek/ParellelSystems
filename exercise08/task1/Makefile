
CFLAGS=-I -Wall -O3 -std=c++11
CFLAGS+=-DEPSILON=20 -DGRIDSIZE=512

all: par2

par2:
	g++ jacobi_2D_par.cpp -o jacobi_2D_par $(CFLAGS) -fopenmp

seq2:
	g++ jacobi_2D_par.cpp -o jacobi_2D_seq $(CFLAGS)
	
clean:
	rm -f jacobi_2D_par jacobi_2D_seq