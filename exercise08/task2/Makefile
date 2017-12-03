
CFLAGS=-Wall -O3 -std=c++11
INCLUDES=-I/usr/local/lib/openmpi/include
LIBS=-L/usr/local/lib/openmpi/lib -lmpi

all: montecarlo_pi_mpi montecarlo_pi

montecarlo_pi_mpi: montecarlo_pi_mpi.cpp
	g++ montecarlo_pi_mpi.cpp -o montecarlo_pi_mpi $(CFLAGS) $(INCLUDES) $(LIBS)

montecarlo_pi: montecarlo_pi.cpp
	g++ montecarlo_pi.cpp -o montecarlo_pi $(CFLAGS)

clean:
	rm -f montecarlo_pi montecarlo_pi_mpi