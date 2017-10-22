#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>


using Matrix = std::vector<std::vector<double>>;

// initializes a square identity matrix of size n x n
Matrix id(unsigned n) {
	Matrix res;
	res.resize(n);
	for(unsigned i=0; i<n; i++) {
		res[i].resize(n);
		for(unsigned j=0; j<n; j++) {
			res[i][j] = (i == j) ? 1 : 0;
		}
	}
	return res;
}

// computes the product of two matrices
Matrix operator*(const Matrix& a, const Matrix& b) {
	unsigned n = a.size();
	Matrix c = id(n);
	int tile = 64;
	for(unsigned i=0; i<n; i+=tile) {
		for(unsigned j=0; j<n; j+=tile) {
			for(unsigned k=0; k<n; k += tile) {
				for(unsigned x = i; x < i+tile; x++){
					for(unsigned y = j; y < j+tile; y++){
						if(k == 0)
							c[x][y] = 0;
						for(unsigned z = k; z < k+tile; z++){
							c[x][y] += a[x][z] * b[z][y];		
						}
					}					
				}
				
			}
		}
	}
	return c;
}



int main(int argc, char** argv) {
	
	auto t_start = std::chrono::high_resolution_clock::now();
	auto start = std::clock();
	
	if(argc!=2) return EXIT_FAILURE;
	unsigned n = atoi(argv[1]);
	if(n==0) return EXIT_FAILURE;

	// create two matrices
	auto a = id(n);
	a[0][0] = 42;
	auto b = id(n);

	// compute the product
	auto c = a * b;
	
	// check that the result is correct
	if(c == a){
		double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
		double t_duration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t_start).count()/1000.0;
		std::cout << "CPU Time: " << duration << std::endl;
		std::cout << "Wall Time: " << t_duration << std::endl;
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
}
