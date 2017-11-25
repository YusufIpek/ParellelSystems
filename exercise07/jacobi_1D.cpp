#include <iostream>
#include <cmath>
#include "jacobi.hpp"


double timestep(double cells[GRIDSIZE], double left, double right)
{
	double change = 0.0, tmp;
	double tmp_old = (left+cells[0]+cells[1])/3;
	change += std::abs(cells[0] - tmp_old);
	
	for (int i = 1; i < GRIDSIZE-1; i++)
	{
		tmp = (cells[i-1]+cells[i]+cells[i+1])/3;
		change += std::abs(cells[i]-tmp);
		cells[i-1] = tmp_old;
		tmp_old = tmp;
	}
	
	tmp = (right+cells[GRIDSIZE-1]+cells[GRIDSIZE-2])/3;
	change += std::abs(cells[GRIDSIZE-1]-tmp);
	cells[GRIDSIZE-2] = tmp_old;
	cells[GRIDSIZE-1] = tmp;
	
	return change;
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
	
	double cells[GRIDSIZE];
	
	for (int i = 0; i < GRIDSIZE; i++)
		cells[i] = 0.0;
	
	double change = EPSILON;
	while (change >= EPSILON)
		change = timestep(cells, left, right);
	
	print_cells(cells);
	
	return 0;
}

