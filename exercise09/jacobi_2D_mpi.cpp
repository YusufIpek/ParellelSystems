#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <omp.h>
#include "mpi.h"
//#include "chrono_timer.h"
#include "jacobi.hpp"


void print_cells(double cells[GRIDSIZE], int row)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
			std::cout << cells[i*GRIDSIZE + j] << " ";

		std::cout << std::endl;
	}
}

void print_row(double cells[GRIDSIZE]) {
	for (int i = 0; i < GRIDSIZE; i++) {
		std::cout << cells[i] << " ";
	}
	std::cout << std::endl;
}


double* timestep_borders(double* cells, double* inner_cells_updated, double& change_return, short rows, double left, double right, double* top, double* bottom)
{
	double* cells_new = static_cast<double*>(calloc(rows*GRIDSIZE, sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	

	// upper left corner
	cells_new[0] = (left + top[0] + cells[0] + cells[1] + cells[GRIDSIZE]) / 5;
	change_return += std::abs(cells_new[0] - cells[0]);

	// top row
	for (int i = 1; i < GRIDSIZE - 1; i++)
	{
		cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i + GRIDSIZE] + top[i]) / 5;
		change_return += std::abs(cells_new[i] - cells[i]);
	}
	
	// upper right corner
	cells_new[GRIDSIZE - 1] = (right + top[GRIDSIZE - 1] + cells[GRIDSIZE - 2] + cells[GRIDSIZE - 1] + cells[2 * GRIDSIZE - 1]) / 5;
	change_return += std::abs(cells_new[GRIDSIZE - 1] - cells[GRIDSIZE - 1]);


	for (int i = 1; i < rows - 1; i++)
	{
		auto a = i*GRIDSIZE;
		auto u = a - GRIDSIZE;
		auto d = a + GRIDSIZE;
		cells_new[a] = (left + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
		change_return += std::abs(cells_new[a] - cells[a]);
		
		for (int j = 1; j < GRIDSIZE - 1; j++)
		{
			cells_new[a + j] = inner_cells_updated[a+j];
		}

		cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right + cells[a - 1] + cells[d - 1 + GRIDSIZE]) / 5;	// right collumn
		change_return += std::abs(cells_new[d - 1] - cells[d - 1]);
	}

	// bottom left corner
	cells_new[GRIDSIZE*(rows - 1)] = (left + bottom[0] + cells[GRIDSIZE*(rows - 2)] + cells[GRIDSIZE*(rows - 1)] + cells[GRIDSIZE*(rows - 1) + 1]) / 5;
	change_return += std::abs(cells_new[GRIDSIZE*(rows - 1)] - cells[GRIDSIZE*(rows - 1)]);


	int tmp_counter = 1;
	for (int i = GRIDSIZE * (rows - 1) + 1; i < (GRIDSIZE*rows) - 1; i++)
	{

		cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i - GRIDSIZE] + bottom[tmp_counter++]) / 5;
		change_return += std::abs(cells_new[i] - cells[i]);
	}

	// bottom right corner
	cells_new[GRIDSIZE*rows - 1] = (bottom[GRIDSIZE - 1] + right + cells[GRIDSIZE*(rows - 1) - 1] + cells[GRIDSIZE*rows - 2] + cells[GRIDSIZE*rows - 1]) / 5;
	change_return += std::abs(cells_new[GRIDSIZE*rows - 1] - cells[GRIDSIZE*rows - 1]);
	return cells_new;
}

double* timestep_inner(double* cells, double& change_return, short rows)
{
	double* cells_new = static_cast<double*>(calloc(rows*GRIDSIZE, sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}

	double change = 0.0;

	for (int i = 1; i < rows - 1; i++)
	{
		auto a = i*GRIDSIZE;
		auto u = a - GRIDSIZE;
		auto d = a + GRIDSIZE;
		
		for (int j = 1; j < GRIDSIZE - 1; j++)
		{
			cells_new[a + j] = (cells[a + j - 1] + cells[a + j] + cells[a + j + 1] + cells[u + j] + cells[d + j]) / 5;
			change += std::abs(cells_new[a + j] - cells[a + j]);
		}

	}

	change_return = change;
	return cells_new;
}


int main(int argc, char* argv[])
{

	if (argc != 5)
	{
		return 1;
	}

	double left, right, *top, *bottom;
	left = atof(argv[1]);
	right = atof(argv[2]);

	top = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));
	bottom = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));

	for (int i = 0; i < GRIDSIZE; i++) {
		top[i] = atof(argv[3]);
		bottom[i] = atof(argv[4]);
	}


	int myrank, nrOfProcesses;
	int msgtag = 1;
	double change_recv = EPSILON;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);


	unsigned mysize = GRIDSIZE / nrOfProcesses;
	if (myrank < GRIDSIZE%nrOfProcesses)	// distribute remainder over threads
	{
		mysize++;
	}

	double* cells_updated = static_cast<double*>(calloc(mysize*GRIDSIZE, sizeof(double)));
	double* cells_old = static_cast<double*>(calloc(mysize*GRIDSIZE, sizeof(double)));


	if (!cells_updated || !cells_old)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		exit(1);
	}

	unsigned nr_of_iter = 0;

	double change = EPSILON;

	int offset_bottom = (mysize - 1)*GRIDSIZE;


	//ChronoTimer t("Jacobi 2D execution time:  ");
	while (change_recv >= EPSILON)
	{
		if (myrank == 0) {
			MPI_Status s;
			MPI_Request req_send, req_recv;
			double* ghost_bottom_recv = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));
			MPI_Isend(&cells_old[offset_bottom], GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_send);
			MPI_Irecv(ghost_bottom_recv, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_recv);
			
			cells_updated = timestep_inner(cells_old, change, mysize);		
			MPI_Wait(&req_recv, &s);
			cells_updated = timestep_borders(cells_old, cells_updated, change, mysize, left, right, top, ghost_bottom_recv);
			
			nr_of_iter++;

			free(ghost_bottom_recv);
			memcpy(cells_old,cells_updated,sizeof(double)*GRIDSIZE*mysize);
			free(cells_updated);
		}
		else if (myrank == (nrOfProcesses - 1)) {
			MPI_Status s;
			MPI_Request req_recv_top_ghost, req_send_top_ghost;
			double* ghost_top_recv = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));
			
			MPI_Irecv(ghost_top_recv, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_recv_top_ghost);
			MPI_Isend(cells_old, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_send_top_ghost);
			
			cells_updated = timestep_inner(cells_old, change, mysize);
			
			MPI_Wait(&req_recv_top_ghost, &s);
			
			cells_updated = timestep_borders(cells_old, cells_updated, change, mysize, left, right, ghost_top_recv, bottom);
			
			free(ghost_top_recv);
			memcpy(cells_old, cells_updated, sizeof(double)*GRIDSIZE*mysize);
			free(cells_updated);
		}
		else {
			double* ghost_top_recv = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));
			double* ghost_bottom_recv = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));

			MPI_Status s;
			MPI_Request req_recv_top_ghost, req_recv_bottom_ghost, req_send_top_ghost, req_send_bottom_ghost;
			MPI_Irecv(ghost_top_recv, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_recv_top_ghost);
			MPI_Isend(cells_old, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_send_top_ghost);
			MPI_Isend(&cells_old[offset_bottom], GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_send_bottom_ghost);
			MPI_Irecv(ghost_bottom_recv, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_recv_bottom_ghost);
			
			cells_updated = timestep_inner(cells_old, change, mysize);
			MPI_Wait(&req_recv_bottom_ghost, &s);
			MPI_Wait(&req_recv_top_ghost, &s);
			
			cells_updated = timestep_borders(cells_old, cells_updated, change, mysize, left, right, ghost_top_recv, ghost_bottom_recv);
			
			free(ghost_top_recv);
			free(ghost_bottom_recv);
			memcpy(cells_old, cells_updated, sizeof(double)*GRIDSIZE*mysize);
			free(cells_updated);
		}
		MPI_Allreduce(&change, &change_recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	}

	MPI_Finalize();



	if (myrank == 0) {
		std::cout << std::endl << "Gridsize: " << GRIDSIZE << "\tEpsilon: " << EPSILON;
		std::cout << std::endl << "Number of iterations: " << nr_of_iter << std::endl;
	}
	return 0;
}