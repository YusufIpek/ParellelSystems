#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <omp.h>
#include "mpi.h"
//#include "chrono_timer.h"
#include "jacobi.hpp"




double* timestep(double* cells, double& change_return, short rows, double left, double right, double* top, double* bottom)
{
	double* cells_new = static_cast<double*>(malloc(rows*GRIDSIZE * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}

	double change = 0.0;

	// upper left corner
	cells_new[0] = (left + top[0] + cells[0] + cells[1] + cells[GRIDSIZE]) / 5;
	change += std::abs(cells_new[0] - cells[0]);

	// top row
	for (int i = 1; i < GRIDSIZE - 1; i++)
	{
		cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i + GRIDSIZE] + top[i]) / 5;
		change += std::abs(cells_new[i] - cells[i]);
	}

	// upper right corner
	cells_new[GRIDSIZE - 1] = (right + top[GRIDSIZE-1] + cells[GRIDSIZE - 2] + cells[GRIDSIZE - 1] + cells[2 * GRIDSIZE - 1]) / 5;
	change += std::abs(cells_new[GRIDSIZE - 1] - cells[GRIDSIZE - 1]);


	for (int i = 1; i < rows - 1; i++)
	{
		auto a = i*GRIDSIZE;
		auto u = a - GRIDSIZE;
		auto d = a + GRIDSIZE;
		cells_new[a] = (left + cells[a] + cells[a + 1] + cells[u] + cells[d]) / 5;	// left collumn
		change += std::abs(cells_new[a] - cells[a]);
		for (int j = 1; j < GRIDSIZE - 1; j++)
		{
			cells_new[a + j] = (cells[a + j - 1] + cells[a + j] + cells[a + j + 1] + cells[u + j] + cells[d + j]) / 5;
			change += std::abs(cells_new[a + j] - cells[a + j]);
		}
		cells_new[d - 1] = (cells[d - 2] + cells[d - 1] + right + cells[a - 1] + cells[d - 1 + GRIDSIZE]) / 5;	// right collumn
		change += std::abs(cells_new[d] - cells[d]);
	}

	// bottom left corner
	cells_new[GRIDSIZE*(rows - 1)] = (left + bottom[0] + cells[GRIDSIZE*(rows - 2)] + cells[GRIDSIZE*(rows - 1)] + cells[GRIDSIZE*(rows - 1) + 1]) / 5;
	change += std::abs(cells_new[GRIDSIZE*(rows - 1)] - cells[GRIDSIZE*(rows - 1)]);

	// bottom row
	int tmp_counter = 0;
	for (int i = GRIDSIZE * (rows - 1) + 1; i < (GRIDSIZE*rows) - 1; i++)
	{
	//	std::cout << "i:" << i << std::endl;
		cells_new[i] = (cells[i - 1] + cells[i] + cells[i + 1] + cells[i - GRIDSIZE] + bottom[tmp_counter++]) / 5;
		change += std::abs(cells_new[i] - cells[i]);
	}

	// bottom right corner
	cells_new[GRIDSIZE*rows - 1] = (bottom[GRIDSIZE-1] + right + cells[GRIDSIZE*(rows - 1) - 1] + cells[GRIDSIZE*rows - 2] + cells[GRIDSIZE*rows - 1]) / 5;
	change += std::abs(cells_new[GRIDSIZE*rows - 1] - cells[GRIDSIZE*rows - 1]);


	change_return = change;
	free(cells);
	free(top);
	free(bottom);
	return cells_new;
}

void print_cells(double cells[GRIDSIZE], int row)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
			std::cout << cells[i*GRIDSIZE + j] << " ";

		std::cout << std::endl;
	}
}

double* get_last_line(double* cells, int rows) {
	double* row = (double*)malloc(GRIDSIZE * sizeof(double));
	for (int i = 0; i < GRIDSIZE; i++) {
		row[i] = cells[((rows-1)*GRIDSIZE)+i];
	}
	return row;
}

double* get_fist_line(double* cells) {
	double* row = (double*)malloc(GRIDSIZE * sizeof(double));
	for (int i = 0; i < GRIDSIZE; i++) {
		row[i] = cells[i];
	}
	return row;
}


double* create_border(double value) {
	double* top_border = (double*)malloc(GRIDSIZE * sizeof(double));
	for (int i = 0; i < GRIDSIZE; i++) {
		top_border[i] = value;
	}
	return top_border;
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
	
	
	unsigned mysize = GRIDSIZE / nrOfProcesses;
	double* cells = static_cast<double*>(calloc(mysize*GRIDSIZE, sizeof(double)));
	

	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		exit(1);
	}

	unsigned nr_of_iter = 0;
	
	double change = EPSILON;
	
	//ChronoTimer t("Jacobi 2D execution time:  ");
	while (change_recv >= EPSILON)
	{
		if (myrank == 0){
			MPI_Status s;
			double* top_border = create_border(top);
			double* bottom_border = get_last_line(cells, mysize);
			double* bottom_border_rec = (double*)malloc(GRIDSIZE * sizeof(double));
		//	std::cout << "Sending data:" << std::endl;
		//	print_cells(bottom_border, 1);
			MPI_Send(bottom_border, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD); //master send bottom row to next process
			MPI_Recv(bottom_border_rec, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &s);
		//	std::cout << "Received Bottom Border: " << std::endl;
		//	print_cells(bottom_border_rec, 1);
		//	std::cout << "Before" << std::endl;
		//	print_cells(cells, mysize);
			cells = timestep(cells, change, mysize, left, right, top_border,bottom_border_rec);
		//	std::cout << "----------------" << std::endl;
		//	print_cells(cells, mysize);
		//	std::cout << "----------------" << std::endl;
			nr_of_iter++;
			//break;
			//std::cout << "Iter: " << nr_of_iter << std::endl;
		}
		else if (myrank == (nrOfProcesses - 1)) {
			MPI_Status s;
			double* top_border_rec = (double*)malloc(GRIDSIZE * sizeof(double));
			double* top_border_send = get_fist_line(cells);
			double* bottom_border = create_border(bottom);
			MPI_Recv(top_border_rec, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &s);
			//std::cout << "Received data:" << std::endl;
			//print_cells(top_border_rec, 1);
			MPI_Send(top_border_send, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD);
			cells = timestep(cells, change, mysize, left, right, top_border_rec, bottom_border);
		}
		else {
			double* top_border_rec = (double*)malloc(GRIDSIZE * sizeof(double));
			double* top_border_send = get_fist_line(cells);
			double* bottom_border_rec = (double*)malloc(GRIDSIZE * sizeof(double));
			double* bottom_border_send = get_last_line(cells, mysize);
			MPI_Status s;
			MPI_Recv(top_border_rec, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD, &s); //receive top border
			MPI_Send(top_border_send, GRIDSIZE, MPI_DOUBLE, myrank - 1, msgtag, MPI_COMM_WORLD);
			MPI_Send(bottom_border_send, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD);
			MPI_Recv(bottom_border_rec, GRIDSIZE, MPI_DOUBLE, myrank + 1, msgtag, MPI_COMM_WORLD, &s); //receive top border
			cells = timestep(cells, change, mysize, left, right, top_border_rec, bottom_border_rec);
		}
		MPI_Allreduce(&change, &change_recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		std::cout << "current epsilon " << change_recv << std::endl;
	}

	MPI_Finalize();

	print_cells(cells, mysize);
	//std::cout << std::endl << "Number of iterations: " << nr_of_iter << std::endl;
	
	return 0;
}