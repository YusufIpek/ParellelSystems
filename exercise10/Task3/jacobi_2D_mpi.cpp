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


double timestep_borders(double* cells, double* cells_new, short rows, short mygridsize, double* left, double* right, double* top, double* bottom, bool global_borders[4])
{
	/* global_borders order: left right top bottom */

	// upper left corner
	cells_new[0] = (left[0] + top[0] + cells[0] + cells[1] + cells[mygridsize]) / 5;
	double change_return = std::abs(cells_new[0] - cells[0]);

	// top row
	if (global_borders[2])
	{
		for (int i = 1; i < mygridsize - 1; i++)
		{
			cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i + mygridsize] + top[0]) / 5;
			change_return += std::abs(cells_new[i] - cells[i]);
		}
		// upper right corner
		cells_new[mygridsize - 1] = (right[0] + top[0] + cells[mygridsize - 2] + cells[mygridsize - 1] + cells[2 * mygridsize - 1]) / 5;
		change_return += std::abs(cells_new[mygridsize - 1] - cells[mygridsize - 1]);
	}
	else 
	{
		for (int i = 1; i < mygridsize - 1; i++)
		{
			cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i + mygridsize] + top[i]) / 5;
			change_return += std::abs(cells_new[i] - cells[i]);
		}
		// upper right corner
		cells_new[mygridsize - 1] = (right[0] + top[mygridsize - 1] + cells[mygridsize - 2] + cells[mygridsize - 1] + cells[2 * mygridsize - 1]) / 5;
		change_return += std::abs(cells_new[mygridsize - 1] - cells[mygridsize - 1]);
	}
	
	
	// left and right collumn
	if (global_borders[0])
	{
		if (global_borders[1]) // left and right borders fixed
		{
			for (int i = 1; i < rows - 1; i++)
			{
				auto a = i*mygridsize;
				auto u = a - mygridsize;
				auto d = a + mygridsize;

				cells_new[a] = (left[0] + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
				change_return += std::abs(cells_new[a] - cells[a]);

				cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right[0] + cells[a - 1] + cells[d - 1 + mygridsize]) / 5;	// right collumn
				change_return += std::abs(cells_new[d - 1] - cells[d - 1]);
			}
		}
		else // just left border fixed
		{
			for (int i = 1; i < rows - 1; i++)
			{
				auto a = i*mygridsize;
				auto u = a - mygridsize;
				auto d = a + mygridsize;

				cells_new[a] = (left[0] + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
				change_return += std::abs(cells_new[a] - cells[a]);

				cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right[i] + cells[a - 1] + cells[d - 1 + mygridsize]) / 5;	// right collumn
				change_return += std::abs(cells_new[d - 1] - cells[d - 1]);
			}
		}
	}
	else
	{
		if (global_borders[1]) // right border fixed
		{
			for (int i = 1; i < rows - 1; i++)
			{
				auto a = i*mygridsize;
				auto u = a - mygridsize;
				auto d = a + mygridsize;

				cells_new[a] = (left[i] + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
				change_return += std::abs(cells_new[a] - cells[a]);

				cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right[0] + cells[a - 1] + cells[d - 1 + mygridsize]) / 5;	// right collumn
				change_return += std::abs(cells_new[d - 1] - cells[d - 1]);
			}
		}
		else // no fixed borders
		{
			for (int i = 1; i < rows - 1; i++)
			{
				auto a = i*mygridsize;
				auto u = a - mygridsize;
				auto d = a + mygridsize;

				cells_new[a] = (left[i] + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
				change_return += std::abs(cells_new[a] - cells[a]);

				cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right[i] + cells[a - 1] + cells[d - 1 + mygridsize]) / 5;	// right collumn
				change_return += std::abs(cells_new[d - 1] - cells[d - 1]);
			}
		}
	}


	// bottom line
	if (global_borders[3]) // bottom border fixed
	{
		// bottom left corner
		if (global_borders[0]) 
			cells_new[mygridsize*(rows - 1)] = (left[0] + bottom[0] + cells[mygridsize*(rows - 2)] + cells[mygridsize*(rows - 1)] + cells[mygridsize*(rows - 1) + 1]) / 5;
		else
			cells_new[mygridsize*(rows - 1)] = (left[rows-1] + bottom[0] + cells[mygridsize*(rows - 2)] + cells[mygridsize*(rows - 1)] + cells[mygridsize*(rows - 1) + 1]) / 5;
		change_return += std::abs(cells_new[mygridsize*(rows - 1)] - cells[mygridsize*(rows - 1)]);

		for (int i = mygridsize * (rows - 1) + 1; i < (mygridsize*rows) - 1; i++)
		{
			cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i - mygridsize] + bottom[0]) / 5;
			change_return += std::abs(cells_new[i] - cells[i]);
		}

		// bottom right corner
		if (global_borders[1])
			cells_new[mygridsize*rows - 1] = (bottom[0] + right[0] + cells[mygridsize*(rows - 1) - 1] + cells[mygridsize*rows - 2] + cells[mygridsize*rows - 1]) / 5;
		else
			cells_new[mygridsize*rows - 1] = (bottom[0] + right[rows-1] + cells[mygridsize*(rows - 1) - 1] + cells[mygridsize*rows - 2] + cells[mygridsize*rows - 1]) / 5;
		change_return += std::abs(cells_new[mygridsize*rows - 1] - cells[mygridsize*rows - 1]);
	}
	else
	{
		// bottom left corner
		if (global_borders[0])
			cells_new[mygridsize*(rows - 1)] = (left[0] + bottom[0] + cells[mygridsize*(rows - 2)] + cells[mygridsize*(rows - 1)] + cells[mygridsize*(rows - 1) + 1]) / 5;
		else
			cells_new[mygridsize*(rows - 1)] = (left[rows - 1] + bottom[0] + cells[mygridsize*(rows - 2)] + cells[mygridsize*(rows - 1)] + cells[mygridsize*(rows - 1) + 1]) / 5;
		change_return += std::abs(cells_new[mygridsize*(rows - 1)] - cells[mygridsize*(rows - 1)]);

		int tmp_counter = 1;
		for (int i = mygridsize * (rows - 1) + 1; i < (mygridsize*rows) - 1; i++)
		{
			cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i - mygridsize] + bottom[tmp_counter++]) / 5;
			change_return += std::abs(cells_new[i] - cells[i]);
		}

		// bottom right corner
		if (global_borders[1])
			cells_new[mygridsize*rows - 1] = (right[0] + bottom[mygridsize - 1] + cells[mygridsize*(rows - 1) - 1] + cells[mygridsize*rows - 2] + cells[mygridsize*rows - 1]) / 5;
		else
			cells_new[mygridsize*rows - 1] = (right[rows - 1] + bottom[mygridsize - 1]  + cells[mygridsize*(rows - 1) - 1] + cells[mygridsize*rows - 2] + cells[mygridsize*rows - 1]) / 5;
		change_return += std::abs(cells_new[mygridsize*rows - 1] - cells[mygridsize*rows - 1]);
	}

	return change_return;
}

double timestep_inner(double* cells, double* cells_new, short rows, short mygridsize)
{
	double change_return = 0.0;

	for (int i = 1; i < rows - 1; i++)
	{
		auto a = i*mygridsize;
		auto u = a - mygridsize;
		auto d = a + mygridsize;
		
		for (int j = 1; j < mygridsize - 1; j++)
		{
			cells_new[a + j] = (cells[a + j - 1] + cells[a + j] + cells[a + j + 1] + cells[u + j] + cells[d + j]) / 5;
			change_return += std::abs(cells_new[a + j] - cells[a + j]);
		}
	}

	return change_return;
}


int main(int argc, char* argv[])
{

	if (argc != 5)
	{
		return 1;
	}

	double left, right, top, bottom;
	left = atof(argv[1]);
	right = atof(argv[2]);
	top = atof(argv[3]);
	bottom = atof(argv[4]);
	

	int myrank, nrOfProcesses;
	int msgtag = 1;
	double change_recv = EPSILON;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);

	double procs_log2_d = log2(nrOfProcesses);
	if (ceilf(procs_log2_d) != procs_log2_d)
	{
		std::cout << "Error: this program only works with a number of processes of a power of 2" << std::endl;
		return 1;
	}
	int procs_log2 = static_cast<int>(procs_log2_d);

	int myrows = GRIDSIZE, mycolumns = GRIDSIZE;
	int hor_fields = 1, ver_fields = 1;

	// splitting up the array
	for (int i = 1; i <= procs_log2; i++)
	{
		if (i % 2 == 1)
		{
			myrows = myrows / 2;
			hor_fields *= 2;
		}
		else
		{
			mycolumns = mycolumns / 2;
			ver_fields *= 2;
		}
	}


	//unsigned mysize = GRIDSIZE / nrOfProcesses;
	//if (myrank < GRIDSIZE%nrOfProcesses)	// distribute remainder over threads
	//{
	//	mysize++;
	//}

	double* cells_updated = static_cast<double*>(malloc(myrows*mycolumns*sizeof(double)));
	double* cells_old = static_cast<double*>(calloc(myrows*mycolumns, sizeof(double)));


	if (!cells_updated || !cells_old)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		return 1;
	}

	unsigned nr_of_iter = 0;

	double change = EPSILON;

	/* global_borders order: left right top bottom */
	bool my_global_borders[4] = { false, false, false, false };

	if (myrank < hor_fields)
		my_global_borders[0] = true;
	if (myrank >= nrOfProcesses - hor_fields)
		my_global_borders[1] = true;
	if (myrank % hor_fields == 0)
		my_global_borders[2] = true;
	if (myrank % hor_fields == hor_fields-1)
		my_global_borders[3] = true;

	MPI_Status s;
	//MPI_Request req_recv_top_ghost, req_recv_bottom_ghost, req_send_top_ghost, req_send_bottom_ghost;
	MPI_Request req_send_left, req_recv_left, req_send_right, req_recv_right, req_send_top, req_recv_top, req_send_bottom, req_recv_bottom;
	
	double *ghost_left_recv, *ghost_right_recv, *ghost_top_recv, *ghost_bottom_recv;
	double *send_left, *send_right;
	ghost_left_recv = static_cast<double*>(malloc(myrows * sizeof(double)));
	send_left = static_cast<double*>(malloc(myrows * sizeof(double)));
	ghost_right_recv = static_cast<double*>(malloc(myrows * sizeof(double)));
	send_right = static_cast<double*>(malloc(myrows * sizeof(double)));
	ghost_top_recv = static_cast<double*>(malloc(mycolumns * sizeof(double)));
	ghost_bottom_recv = static_cast<double*>(malloc(mycolumns * sizeof(double)));
	
	//ChronoTimer t("Jacobi 2D execution time:  ");
	while (change_recv >= EPSILON)
	{
		if (nrOfProcesses == 1) {
			
			change = timestep_inner(cells_old, cells_updated, GRIDSIZE, GRIDSIZE);
			change += timestep_borders(cells_old, cells_updated, GRIDSIZE, GRIDSIZE, &left, &right, &top, &bottom, my_global_borders);
			//memcpy(cells_old, cells_updated, sizeof(double)*GRIDSIZE*mysize);
			double* temp = cells_old;
			cells_old = cells_updated;
			cells_updated = temp;
			
		}
		else {
			

			if (!my_global_borders[0])
			{
				for (int k = 0; k < myrows; k++)
					send_left[k] = cells_old[k*mycolumns];
				// here send to: myrank-hor_fields
				MPI_Isend(send_left, myrows, MPI_DOUBLE, myrank - hor_fields, msgtag, MPI_COMM_WORLD, &req_send_left);
				MPI_Irecv(ghost_left_recv, myrows, MPI_DOUBLE, myrank - hor_fields, msgtag, MPI_COMM_WORLD, &req_recv_left);
			}
			else
				ghost_left_recv[0] = left;

			if (!my_global_borders[1])
			{
				for (int k = 0; k < myrows; k++)
					send_right[k] = cells_old[(k+1)*mycolumns - 1];
				// here send to: myrank+hor_fields
				MPI_Isend(send_right, myrows, MPI_DOUBLE, myrank + hor_fields, msgtag, MPI_COMM_WORLD, &req_send_right);
				MPI_Irecv(ghost_right_recv, myrows, MPI_DOUBLE, myrank + hor_fields, msgtag, MPI_COMM_WORLD, &req_recv_right);
			}
			else
				ghost_right_recv[0] = right;

			if (!my_global_borders[2])
			{
				// here send top line to: myrank-1 
				MPI_Isend(cells_old, mycolumns, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_send_top);
				MPI_Irecv(ghost_top_recv, mycolumns, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &req_recv_top);
			}
			else
				ghost_top_recv[0] = top;

			if (!my_global_borders[3])
			{
				// here send bottom line to: myrank+1
				MPI_Isend(&cells_old[(myrows-1)*mycolumns], mycolumns, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_send_bottom);
				MPI_Irecv(ghost_bottom_recv, mycolumns, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &req_recv_bottom);
			}
			else
				ghost_bottom_recv[0] = bottom;

			change = timestep_inner(cells_old, cells_updated, myrows, mycolumns);
			
			if (!my_global_borders[0])
				MPI_Wait(&req_recv_left, &s);
			if (!my_global_borders[1])
				MPI_Wait(&req_recv_right, &s);
			if (!my_global_borders[2])
				MPI_Wait(&req_recv_top, &s);
			if (!my_global_borders[3])
				MPI_Wait(&req_recv_bottom, &s);
			
			change += timestep_borders(cells_old, cells_updated, myrows, mycolumns, ghost_left_recv, ghost_right_recv, ghost_top_recv, ghost_bottom_recv, my_global_borders);
			
			double* temp = cells_old;
			cells_old = cells_updated;
			cells_updated = temp;
			
		}
		
		if (myrank == 0)
			nr_of_iter++;
		
		MPI_Allreduce(&change, &change_recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	} // end while

	free(ghost_left_recv);
	free(ghost_right_recv);
	free(ghost_top_recv);
	free(ghost_bottom_recv);
	free(cells_old);
	free(cells_updated);
	
	MPI_Finalize();

	if (myrank == 0) {
		std::cout << std::endl << "Gridsize: " << GRIDSIZE << "\tEpsilon: " << EPSILON;
		std::cout << std::endl << "Number of iterations: " << nr_of_iter << std::endl;
	}
	return 0;
}
