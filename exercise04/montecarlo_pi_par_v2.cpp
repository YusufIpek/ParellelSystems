#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <omp.h>
#include "chrono_timer.h"

#define R RAND_MAX


unsigned int rand_foo;
#pragma omp threadprivate(rand_foo)

long R_sqr = static_cast<long>(R)*R;

bool calc_point()
{
	long x, y;
	x = rand_r(&rand_foo);
	y = rand_r(&rand_foo);
	
	if (x*x + y*y < R_sqr) 
		return true;
	else
		return false;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: montecarlo_pi_par N" << std::endl;
		return 1;
	}
	srand(time(NULL));
	
	long n = atol(argv[1]);	
	long m = 0; // number of points inside circle
	
	ChronoTimer t("Monte Carlo pi estimation parallel v2:  ");
  #pragma omp parallel
  {
    rand_foo = omp_get_thread_num();
    #pragma omp for reduction(+:m)
  	for (long i = 0; i < n; i++)
  	{
  		if (calc_point())
  		{
  			m++;
  		}
  	}
  }
	
	double pi = static_cast<double>(m*4)/n;
	
	std::cout << "estimation of pi:  " << pi << std::endl;
	
	return 0;
}
