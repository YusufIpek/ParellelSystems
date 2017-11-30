#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cmath>

#include "chrono_timer.h"

#define R RAND_MAX

unsigned int rand_foo;

long R_sqr = static_cast<long>(R)*R;

bool calc_point()
{
	long x, y;
	x = rand_r(&rand_foo);
	y = rand_r(&rand_foo);
	
	if (x*x + y*y < R_sqr) 
		return true;
	else
		return false;
}

int main(int argc, char* argv[])
{
	
	if (argc != 2)
	{
		std::cout << "Usage: montecarlo_pi_par N" << std::endl;
		return 1;
	}
	
	long n = atol(argv[1]);
	long m = 0, m_recv = 0; // number of points inside circle
	
	int msgtag = MPI_ANY_TAG;
	int myrank;
	int nrOfProcesses;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	// find mpi process rank
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);
	
	srand(static_cast<unsigned>(pow(13,myrank))+2017);
	
	/*
	if (myrank == 0)	// master
	{
		unsigned x;
		MPI_Status s;
		//MPI_Recv(&x, 1, MPI_UNSIGNED_INT, MPI_ANY_SOURCE,msgtag,MPI_COMM_WORLD,&s);
		MPI_Recv(&x, 1, MPI_UNSIGNED_INT, 1,msgtag,MPI_COMM_WORLD,&s);
		
	}
	else if (myrank == 1)	// slave
	{
		unsigned x;
		MPI_Send(&x, 1, MPI_UNSIGNED_INT, 0, msgtag, MPI_COMM_WORLD);
	}
	*/
	
	for (long i = 0; i < n/nrOfProcesses; i++)
  	{
  		long x, y;
		x = rand();
		y = rand();
	
		if (x*x + y*y < R_sqr)
			m++;
  	}
  	
  	MPI_Reduce(&m, &m_recv, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if (myrank == 0)	// master
	{	
		double pi = static_cast<double>(m_recv*4)/n;
		
		std::cout << "estimation of pi:  " << pi << std::endl;
	}
	
	MPI_Finalize();
	
	return 0;
}
