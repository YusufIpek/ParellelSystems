#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

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
	for(unsigned i=0; i<n; ++i) {
		for(unsigned j=0; j<n; ++j) {
			c[i][j] = 0;
			for(unsigned k=0; k<n; ++k) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return c;
}


int main(int argc, char** argv) {
	
	std::clock_t start;
	start = std::clock();
	
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
		std::cout << "Time: " << duration << std::endl;
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
}
