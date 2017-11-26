#include <iostream>
#include <cmath>
#include <omp.h>
//#include "chrono_timer.h"
#include "jacobi.hpp"


double* timestep(double* cells, double& change_return, double left, double right, double top, double bottom, double front, double back)
{
	double* cells_new = static_cast<double*>(malloc(GRIDSIZE*GRIDSIZE*GRIDSIZE * sizeof(double)));
	if (!cells_new)
	{
		std::cout << "Error: could not malloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	double change = 0.0;
	
	for (int k = 0; k < GRIDSIZE; k++)
	{
		int ka = k*(GRIDSIZE*GRIDSIZE);	// actual plane
		int kf = (k-1)*(GRIDSIZE*GRIDSIZE);	// plane in front
		int kb = (k+1)*(GRIDSIZE*GRIDSIZE);	// plane in back
		
		if (k == 0)	// replace kf by front
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + top + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + bottom + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + front + cells[kb+ia+j] )/7;
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + front + cells[kb+ia+j] )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
					}
				}
				
			}
			
		}
		else if (k == GRIDSIZE-1)	// replace kb by back
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + top + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + top + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + bottom + cells[kf+ia+j] + back )/7;
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
							cells_new[ka+ia+j] = ( left + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else if (j == GRIDSIZE-1)
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + right + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
							change += std::abs(cells_new[ka+ia+j] - cells[ka+ia+j]);
						}
						else
						{
							cells_new[ka+ia+j] = ( cells[ka+ia+j-1] + cells[ka+ia+j] + cells[ka+ia+j+1] + cells[ka+iu+j] + cells[ka+id+j] + cells[kf+ia+j] + back )/7;
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
	
	double left, right, top, bottom, front, back;
	left = atof(argv[1]);
	right = atof(argv[2]);
	top = atof(argv[3]);
	bottom = atof(argv[4]);
	front = atof(argv[5]);
	back = atof(argv[6]);
	
	double* cells = static_cast<double*>(calloc(GRIDSIZE*GRIDSIZE*GRIDSIZE, sizeof(double)));
	if (!cells)
	{
		std::cout << "Error: could not calloc!" << std::endl; fflush(stdout);
		exit(1);
	}
	
	unsigned nr_of_iter = 0;
	double change = EPSILON;
	
	//ChronoTimer t("Jacobi 3D execution time:  ");
	while (change >= EPSILON)
	{
		nr_of_iter++;
		cells = timestep(cells, change, left, right, top, bottom, front, back);
	}
	
	print_cells(cells);
	std::cout << std::endl << "Number of iterations: " << nr_of_iter << std::endl;
	
	return 0;
}

