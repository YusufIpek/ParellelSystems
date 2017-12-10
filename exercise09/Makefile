CFLAGS=-Wall -O3 -std=c++11 -march=native
CFLAGS_M=-DEPSILON=10 -DGRIDSIZE=512
CFLAGS_M2=-DEPSILON=100 -DGRIDSIZE=768

all: jacobi_2D_512_mpi jacobi_2D_768_mpi

jacobi_2D_512_mpi: jacobi_2D_mpi.cpp
	mpiCC jacobi_2D_mpi.cpp -o jacobi_2D_512_mpi $(CFLAGS) $(CFLAGS_M)
	
jacobi_2D_768_mpi: jacobi_2D_mpi.cpp
	mpiCC jacobi_2D_mpi.cpp -o jacobi_2D_768_mpi $(CFLAGS) $(CFLAGS_M2)

clean:
	rm -f jacobi_2D_512_mpi jacobi_2D_768_mpi