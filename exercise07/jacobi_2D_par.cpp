#include <iostream>
#include <cmath>
#include <omp.h>
//#include "chrono_timer.h"
#include "jacobi.hpp"


double* timestep(double* cells, double& change_return, double left, double right, double top, double bottom)
{
	double* cells_new = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = 0.0;
	
	// upper left corner
	cells_new[0] = (left+top+cells[0]+cells[1]+cells[GRIDSIZE])/5;
	change += std::abs(cells_new[0] - cells[0]);
	
	// top row
	for (int i = 1; i < GRIDSIZE-1; i++)
	{
		cells_new[i] = (cells[i-1]+cells[i]+cells[i+1]+cells[i+GRIDSIZE]+top)/5;
		change += std::abs(cells_new[i] - cells[i]);
	}
	
	// upper right corner
	cells_new[GRIDSIZE-1] = (right+top+cells[GRIDSIZE-2]+cells[GRIDSIZE-1]+cells[2*GRIDSIZE-1])/5;
	change += std::abs(cells_new[GRIDSIZE-1] - cells[GRIDSIZE-1]);
	
	#pragma omp parallel for reduction(+:change)
	for (int i = 1; i < GRIDSIZE-1; i++)
	{
		auto a = i*GRIDSIZE;
		auto u = a-GRIDSIZE;
		auto d = a+GRIDSIZE;
		cells_new[a] = (left+cells[a]+cells[a+1]+cells[u]+cells[d])/5;	// left collumn
		change += std::abs(cells_new[a] - cells[a]);
		for (int j = 1; j < GRIDSIZE-1; j++)
		{
			cells_new[a+j] = (cells[a+j-1]+cells[a+j]+cells[a+j+1]+cells[u+j]+cells[d+j])/5;
			change += std::abs(cells_new[a+j] - cells[a+j]);
		}
		cells_new[d-1] = (cells[d-2]+cells[d-1]+right+cells[a-1]+cells[d-1+GRIDSIZE])/5;	// right collumn
		change += std::abs(cells_new[d] - cells[d]);
	}
	
	// bottom left corner
	cells_new[GRIDSIZE*(GRIDSIZE-1)] = (left+bottom+cells[GRIDSIZE*(GRIDSIZE-2)]+cells[GRIDSIZE*(GRIDSIZE-1)]+cells[GRIDSIZE*(GRIDSIZE-1)+1])/5;
	change += std::abs(cells_new[GRIDSIZE*(GRIDSIZE-1)] - cells[GRIDSIZE*(GRIDSIZE-1)]);
	
	// bottom row
	for (int i = GRIDSIZE*(GRIDSIZE-1)+1; i < GRIDSIZE*GRIDSIZE-1; i++)
	{
		cells_new[i] = (cells[i-1]+cells[i]+cells[i+1]+cells[i-GRIDSIZE]+bottom)/5;
		change += std::abs(cells_new[i] - cells[i]);
	}
	
	// bottom right corner
	cells_new[GRIDSIZE*GRIDSIZE-1] = (bottom+right+cells[GRIDSIZE*(GRIDSIZE-1)-1]+cells[GRIDSIZE*GRIDSIZE-2]+cells[GRIDSIZE*GRIDSIZE-1])/5;
	change += std::abs(cells_new[GRIDSIZE*GRIDSIZE-1]-cells[GRIDSIZE*GRIDSIZE-1]);
	
	
	change_return = change;
	free(cells);
	return cells_new;
}

void print_cells(double cells[GRIDSIZE])
{
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
			std::cout << cells[i*GRIDSIZE+j] << " ";
	
		std::cout << std::endl;
	}
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
	
	double* cells = static_cast<double*>(calloc(GRIDSIZE*GRIDSIZE, sizeof(double)));
	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	unsigned nr_of_iter = 0;
	double change = EPSILON;
	
	//ChronoTimer t("Jacobi 2D execution time:  ");
	while (change >= EPSILON)
	{
		nr_of_iter++;
		cells = timestep(cells, change, left, right, top, bottom);
	}
	
	print_cells(cells);
	std::cout << std::endl << "Number of iterations: " << nr_of_iter << std::endl;
	
	return 0;
}

