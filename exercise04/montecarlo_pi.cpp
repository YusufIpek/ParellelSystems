#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <limits>
#include "chrono_timer.h"

#define R RAND_MAX

long R_sqr = static_cast<long>(R)*R;

bool calc_point()
{
	long x, y;
	x = rand();
	y = rand();
	//std::cout << "x: " << x*x << " y: " << y*y << " R: " << R_sqr << std::endl;
	if (x*x + y*y < R_sqr) 
		return true;
	else
		return false;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: montecarlo_pi N" << std::endl;
		return 1;
	}
	
	long n = atol(argv[1]);
	
	srand(time(NULL));
	/*
	std::cout << "RAND_MAX: " << RAND_MAX << std::endl;
	std::cout << "INT_MAX: " << std::numeric_limits<int>::max() << std::endl;
	std::cout << "LONG_MAX: " << std::numeric_limits<long>::max() << std::endl;
	*/
	
	
	long m = 0; // number of points inside circle
	
	ChronoTimer t("Monte Carlo pi estimation:  ");
	for (long i = 0; i < n; i++)
	{
		if (calc_point())
			m++;
	}
	
	double pi = static_cast<double>(m*4)/n;
	
	std::cout << "estimation of pi:  " << pi << std::endl;
	
	return 0;
}