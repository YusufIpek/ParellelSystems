#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <omp.h>
#include "chrono_timer.h"

#define R RAND_MAX
#define OUR_NUM_THREADS 4

long R_sqr = static_cast<long>(R)*R;

bool calc_point()
{
	long x, y;
	x = rand();
	y = rand();
	
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
	long m[OUR_NUM_THREADS] = { 0 }; // number of points inside circle
	
	ChronoTimer t("Monte Carlo pi estimation parallel v2:  ");
	#pragma omp parallel for num_threads(OUR_NUM_THREADS)
	for (long i = 0; i < n; i++)
	{
		if (calc_point())
		{
			m[omp_get_thread_num()]++;
		}
	}
	
	long m_final = 0;
	for (int j = 0; j < OUR_NUM_THREADS; j++)
	  m_final += m[j];
	
	double pi = static_cast<double>(m_final*4)/n;
	
	std::cout << "estimation of pi:  " << pi << std::endl;
	
	return 0;
}