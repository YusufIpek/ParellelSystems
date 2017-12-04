#include <iostream>
#include <cmath>
#include <omp.h>
#include <mpi.h>
//#include "chrono_timer.h"
#include "jacobi.hpp"


double* timestep(double* cells, int mysize, double& change_return, double left, double right, double top, double bottom, double* front, double* back)
{
	double* cells_new = static_cast<double*>(malloc(mysize*GRIDSIZE*GRIDSIZE * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = 0.0;
	
	for (int k = 0; k < mysize; k++)
	{
		int ka = k*(GRIDSIZE*GRIDSIZE);	// actual plane
		int kf = (k-1)*(GRIDSIZE*GRIDSIZE);	// plane in front
		int kb = (k+1)*(GRIDSIZE*GRIDSIZE);	// plane in back
		
		if (k == 0)	// replace kf by front[ia+j]
		{
			for (int i = 0; i < GRIDSIZE; i++)
			{
				int ia = i*GRIDSIZE;	// actual row
				int iu = (i-1)*GRIDSIZE;	// one row up
				int id = (i+1)*GRIDSIZE;	// one row down
				
				if (i == 0)	// replace iu by top
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + top + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else if (i == GRIDSIZE-1)	// replace id by bottom
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + bottom + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + front[ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				
			}
			
		}
		else if (k == mysize-1)	// replace kb by back[ia+j]
		{
			for (int i = 0; i < GRIDSIZE; i++)
			{
				int ia = i*GRIDSIZE;	// actual row
				int iu = (i-1)*GRIDSIZE;	// one row up
				int id = (i+1)*GRIDSIZE;	// one row down
				
				if (i == 0)	// replace iu by top
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + top + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else if (i == GRIDSIZE-1)	// replace id by bottom
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back[ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				
			}
			
		}
		else
		{
			for (int i = 0; i < GRIDSIZE; i++)
			{
				int ia = i*GRIDSIZE;	// actual row
				int iu = (i-1)*GRIDSIZE;	// one row up
				int id = (i+1)*GRIDSIZE;	// one row down
				
				if (i == 0)
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + top + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else if (i == GRIDSIZE-1)
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + bottom + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				else
				{
					for (int j = 0; j < GRIDSIZE; j++)
					{
						if (j == 0)
						{
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				
			}
			
		}
	}
	
	
	change_return = change;
	free(cells);
	return cells_new;
}

void print_cells(double* cells)
{
	for (int k = 0; k < GRIDSIZE; k++)
	{
		std::cout << "plane " << k << std::endl;
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
				std::cout << cells[k*GRIDSIZE*GRIDSIZE+i*GRIDSIZE+j] << " ";
		
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{

	if (argc != 7)
	{
		return 1;
	}
	
	double left, right, top, bottom, *front, *back;
	left = atof(argv[1]);
	right = atof(argv[2]);
	top = atof(argv[3]);
	bottom = atof(argv[4]);
	
	front = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
	back = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
	for (int i = 0; i < GRIDSIZE*GRIDSIZE; i++)
	{
		front[i] = atof(argv[5]);
		back[i] = atof(argv[6]);
	}
	
	unsigned nrOfIter = 0;
	int msgtag = 1;
	int myrank;
	int nrOfProcesses;
	double change, change_recv = EPSILON;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	// find mpi process rank
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);

	int mysize = GRIDSIZE/nrOfProcesses;	// only really accurate when GRIDSIZE % nrOfProcesses == 0
	if( myrank < GRIDSIZE%nrOfProcesses )	// distribute remainder over threads
	{
		mysize++;
	}
	
	double* cells = static_cast<double*>(calloc(mysize*GRIDSIZE*GRIDSIZE, sizeof(double)));
	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		return 1;
	}
	
	int offset_back = (mysize-1)*GRIDSIZE*GRIDSIZE;
	
	//ChronoTimer t("Jacobi 3D execution time:  ");
	
	while (change_recv >= EPSILON)
	{
		
		
		
		if (myrank == 0)
		{
			double* myghostback = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
			//MPI_Send(void *buf, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm)
			MPI_Send(&cells[offset_back], GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD);
			MPI_Status s;
			MPI_Recv(myghostback, GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD, &s);
			
			cells = timestep(cells, mysize, change, left, right, top, bottom, front, myghostback);
			nrOfIter++;
			free(myghostback);
		}
		else if (myrank == nrOfProcesses-1)
		{
			double* myghostfront = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
			MPI_Status s;
			MPI_Recv( myghostfront , GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD, &s);
			MPI_Send(cells, GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD);
			
			cells = timestep(cells, mysize, change, left, right, top, bottom, myghostfront, back);
			
			free(myghostfront);
		}
		else
		{
			double* myghostfront = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
			double* myghostback = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*sizeof(double)));
			MPI_Status s;
			MPI_Recv( myghostfront , GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD, &s);
			MPI_Send(cells, GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank-1, msgtag, MPI_COMM_WORLD);
			MPI_Send(&cells[offset_back], GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD);
			MPI_Recv(myghostback, GRIDSIZE*GRIDSIZE, MPI_DOUBLE, myrank+1, msgtag, MPI_COMM_WORLD, &s);
			
			cells = timestep(cells, mysize, change, left, right, top, bottom, myghostfront, myghostback);
			
			free(myghostfront);
			free(myghostback);
		}
		
		
		
		MPI_Allreduce(&change, &change_recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);		// Allreduce broadcasts the change
		
	}
	
	MPI_Finalize();
	
	//print_cells(cells);
	if (myrank == 0)
	{
		std::cout << std::endl << "Gridsize: " << GRIDSIZE << "\tEpsilon: " << EPSILON;
		std::cout << std::endl << "Number of iterations: " << nrOfIter << std::endl;
	}
	
	return 0;
}

