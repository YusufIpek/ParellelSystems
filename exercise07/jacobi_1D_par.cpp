#include <iostream>
#include <cmath>
#include <omp.h>
#include "jacobi.hpp"


double* timestep(double* cells, double& change_return, double left, double right)
{
	double* cells_new = static_cast<double*>(malloc(GRIDSIZE * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = 0.0, tmp;
	cells_new[0] = (left+cells[0]+cells[1])/3;
	change += std::abs(cells_new[0] - cells[0]);
	
	#pragma omp parallel for reduction(+:change)
	for (int i = 1; i < GRIDSIZE-1; i++)
	{
		cells_new[i] = (cells[i-1]+cells[i]+cells[i+1])/3;
		change += std::abs(cells_new[i] - cells[i]);
	}
	
	cells_new[GRIDSIZE-1] = (right+cells[GRIDSIZE-1]+cells[GRIDSIZE-2])/3;
	change += std::abs(cells_new[GRIDSIZE-1]-cells[GRIDSIZE-1]);
	
	change_return = change;
	free(cells);
	return cells_new;
}

void print_cells(double cells[GRIDSIZE])
{
	for (int i = 0; i < GRIDSIZE; i++)
		std::cout << cells[i] << " ";
	
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{

	if (argc != 3)
	{
		return 1;
	}
	
	double left, right;
	left = atof(argv[1]);
	right = atof(argv[2]);
	
	double* cells = static_cast<double*>(calloc(GRIDSIZE, sizeof(double)));
	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = EPSILON;
	while (change >= EPSILON)
		cells = timestep(cells, change, left, right);
	
	print_cells(cells);
	
	return 0;
}

