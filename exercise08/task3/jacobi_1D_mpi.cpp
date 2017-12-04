#include <iostream>
#include <cmath>
#include <mpi.h>
#include "jacobi.hpp"


double* timestep(double* cells, unsigned gridsize, double& change_return, double left, double right)
{
	double* cells_new = static_cast<double*>(malloc(gridsize * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = 0.0;
	cells_new[0] = (left+cells[0]+cells[1])/3;
	change += std::abs(cells_new[0] - cells[0]);
	
	for (unsigned i = 1; i < gridsize-1; i++)
	{
		cells_new[i] = (cells[i-1]+cells[i]+cells[i+1])/3;
		change += std::abs(cells_new[i] - cells[i]);
	}
	
	cells_new[gridsize-1] = (right+cells[gridsize-1]+cells[gridsize-2])/3;
	change += std::abs(cells_new[gridsize-1]-cells[gridsize-1]);
	
	change_return = change;
	free(cells);
	return cells_new;
}

int main(int argc, char* argv[])
{

	if (argc != 3)
	{
		return 1;
	}
	
	double left, right, change, change_recv = EPSILON;
	left = atof(argv[1]);
	right = atof(argv[2]);
	
	unsigned nrOfIter = 0;
	int msgtag = 1;
	int myrank;
	int nrOfProcesses;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	// find mpi process rank
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);

	// only really accurate when GRIDSIZE % nrOfProcesses == 0
	unsigned mysize = GRIDSIZE/nrOfProcesses;
	
	double* cells = static_cast<double*>(calloc(mysize, sizeof(double)));
	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		return 1;
	}
	
	while (change_recv >= EPSILON)
	{
	
		if ( myrank == 0 )	// master
		{
			double myright = cells[mysize-1];
			double myghostright;
			//MPI_Send(void *buf, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm)
			MPI_Send(&myright, 1, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD);
			MPI_Status s;
			MPI_Recv(&myghostright, 1, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD, &s);
			cells = timestep(cells, mysize, change, left, myghostright);
			nrOfIter++;
		}
		else if ( myrank == nrOfProcesses-1 ) // last one
		{
			double myleft = cells[0];
			double myghostleft;
			MPI_Status s;
			MPI_Recv(&myghostleft, 1, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD, &s);
			MPI_Send(&myleft, 1, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD);
			cells = timestep(cells, mysize, change, myghostleft, right);
		}
		else	// interior
		{
			double myleft = cells[0];
			double myright = cells[mysize-1];
			double myghostleft, myghostright;
			MPI_Status s;
			MPI_Recv(&myghostleft, 1, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD, &s);
			MPI_Send(&myleft, 1, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD);
			MPI_Send(&myright, 1, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD);
			MPI_Recv(&myghostright, 1, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD, &s);
			cells = timestep(cells, mysize, change, myghostleft, myghostright);
		}

		MPI_Allreduce(&change, &change_recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);		// Allreduce broadcasts the change	
		
	}

	MPI_Finalize();
	
	if (myrank == 0)
		std::cout << "Number of Iterations: " << nrOfIter << std::endl; fflush(stdout);
	
	return 0;
}

