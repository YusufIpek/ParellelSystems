
CFLAGS=-I -Wall -O3 -std=c++11
CFLAGS+=-DEPSILON=0.001 -DGRIDSIZE=16

jacobi_1D:
	g++ jacobi_1D.cpp -o jacobi_1D $(CFLAGS)
	
par1:
	g++ jacobi_1D_par.cpp -o jacobi_1D_par $(CFLAGS) -fopenmp
	
par2:
	g++ jacobi_2D_par.cpp -o jacobi_2D_par $(CFLAGS) -fopenmp

par3:
	g++ jacobi_3D_par.cpp -o jacobi_3D_par $(CFLAGS) -fopenmp