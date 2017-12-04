
CFLAGS=-Wall -O3 -std=c++11
CFLAGS+=-DEPSILON=0.001 -DGRIDSIZE=1024
INCLUDES=-I/usr/local/lib/openmpi/include
LIBS=-L/usr/local/lib/openmpi/lib -lmpi

all: jacobi_1D_mpi

jacobi_1D_mpi: jacobi_1D_mpi.cpp
	g++ jacobi_1D_mpi.cpp -o jacobi_1D_mpi $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f jacobi_1D_mpi