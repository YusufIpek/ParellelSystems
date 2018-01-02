#include <vector>
#include <cstdlib>
#include <iostream>
#include "mpi.h"


#if !defined(NESTED_VECTOR) && !defined(CONTIGUOUS_WITH_MULTIPLICATION) && !defined(CONTIGUOUS_WITH_INDIRECTION)
#define CONTIGUOUS_WITH_MULTIPLICATION
#endif

#if defined(NESTED_VECTOR)

struct Matrix : std::vector<std::vector<double>> {
	unsigned matHeight;
	unsigned matWidth;
	unsigned size;
	Matrix() : matHeight(0), matWidth(0) {}
	Matrix(unsigned height, unsigned width) : matHeight(height), matWidth(width) {
		resize(height);
		for (unsigned i = 0; i < height; ++i) {
			(*this)[i].resize(width);
		}
	}
	double& operator()(unsigned i, unsigned j) {
		return (*this)[i][j];
	}
	const double& operator()(unsigned i, unsigned j) const {
		return (*this)[i][j];
	}
};

#elif defined(CONTIGUOUS_WITH_MULTIPLICATION)

struct Matrix : std::vector<double> {
	unsigned matHeight;
	unsigned matWidth;
	Matrix() : matHeight(0), matWidth(0) {}
	Matrix(unsigned n, unsigned m) : matHeight(n), matWidth(m) {
		resize(n*m);
	}
	double& operator()(unsigned i, unsigned j) {
		return (*this)[i*matWidth + j];
	}
	const double& operator()(unsigned i, unsigned j) const {
		return (*this)[i*matWidth + j];
	}
};

#elif defined(CONTIGUOUS_WITH_INDIRECTION)

struct Matrix : std::vector<double> {
	unsigned matSize;
	std::vector<double*> indirection;
	Matrix() : matSize(0) {}
	Matrix(unsigned n) : matSize(n) {
		resize(n*n);
		indirection.resize(n);
		for (unsigned i = 0; i < n; ++i) {
			indirection[i] = &(*this)[i*n];
		}
	}
	double& operator()(unsigned i, unsigned j) {
		return indirection[i][j];
	}
	const double& operator()(unsigned i, unsigned j) const {
		return indirection[i][j];
	}
};

#endif

// initializes a matrix of size n x n
template<typename F>
Matrix init(unsigned height, unsigned width, F initFun) {
	Matrix res(height, width);
	for (unsigned i = 0; i < height; i++) {
		for (unsigned j = 0; j < width; j++) {
			res(i, j) = initFun(i, j);
		}
	}
	return res;
}

void print(const Matrix& c) {
	int height = c.matHeight;
	int width = c.matWidth;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			std::cout << c(i, j) << "\t";
		}
		std::cout << std::endl;
	}
}


// computes the product of two matrices
Matrix mul(const Matrix& a, const Matrix& b, int height, int width, int rowBoundaryL, int rowBoundaryH, int columnBoundaryL, int columnBoundaryH, unsigned n) {

	Matrix c = init(height, width, [](unsigned, unsigned) { return 0; });
	for (unsigned i = rowBoundaryL, ix = 0; i < rowBoundaryH; ++i, ix++) {
		for (unsigned j = columnBoundaryL, jx = 0; j < columnBoundaryH; ++j, jx++) {
			for (unsigned k = 0; k < n; ++k) {	
				c(ix, jx) += a(i, k) * b(k, j);
			}
		}
	}
	
	return c;
}


int main(int argc, char** argv) {

	if (argc != 2) return EXIT_FAILURE;
	unsigned n = atoi(argv[1]);
	if (n == 0) return EXIT_FAILURE;


	int myrank, nrOfProcesses;
	int msgtag = 1;
	int rowBoundaryL = 0, rowBoundaryH = 0, columnBoundaryL = 0, columnBoundaryH = 0;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);


	if (n % 2 != 0) {
		std::cout << "Error: matrix size must be an even number" << std::endl;
		return 1;
	}

	double procs_log2_d = log2(nrOfProcesses);
	if (ceilf(procs_log2_d) != procs_log2_d)
	{
		std::cout << "Error: this program only works with a number of processes of a power of 2" << std::endl;
		return 1;
	}
	int procs_log2 = static_cast<int>(procs_log2_d);

	int myrows = n, mycolumns = n;
	int hor_fields = 1, ver_fields = 1;

	// splitting up the array
	for (int i = 1; i <= procs_log2; i++)
	{
		if (i % 2 == 1)
		{
			myrows = myrows / 2;
			hor_fields *= 2;
		}
		else
		{
			mycolumns = mycolumns / 2;
			ver_fields *= 2;
		}
	}

//	if(myrank == 0)
//		std::cout << "Hor:" << hor_fields << " Ver:" << ver_fields << std::endl;

	//compute the boundaries for each process
	int counter = 0;
	for (int i = 0; i < hor_fields; i++) {
		for (int j = 0; j < ver_fields; j++) {
			if (myrank == counter++) {
				rowBoundaryL = myrows * i;
				rowBoundaryH = rowBoundaryL + myrows;
				columnBoundaryL = mycolumns * j;
				columnBoundaryH = columnBoundaryL + mycolumns;
				
			}
		}
	}
//	if (myrank == 0) {
//		std::cout << "Myrows:" << myrows << " Mycolumns:" << mycolumns << std::endl;
//	}
//	std::cout << "Rank: " << myrank << " RowL:" << rowBoundaryL << " RowH:" << rowBoundaryH <<
//				" ColumnL:" << columnBoundaryL << " ColumnU:" << columnBoundaryH << std::endl;



	//std::vector<double> receive_buffer(n*n, -1);
	Matrix receive_buffer(n,n);

	// create two matrices
	auto a = init(n, n, [](unsigned i, unsigned j) { return (i == j)? 42.0 : 0.0; });
	auto b = init(n, n, [](unsigned i, unsigned j) { return (i == j) ? 1.0 : 0.0; });

	Matrix c = mul(a, b, myrows, mycolumns, rowBoundaryL, rowBoundaryH, columnBoundaryL, columnBoundaryH, n);

	//Gather all elements from each process
	MPI_Gather(&c[0], myrows*mycolumns, MPI_DOUBLE, &receive_buffer[0], myrows*mycolumns, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	if (myrank == 0) {
		print(receive_buffer);
		// check that the result is correct
		auto ret = (a == receive_buffer) ? EXIT_SUCCESS : EXIT_FAILURE;
		std::cout << "Ret: " << ret << std::endl;
		return ret;
	}
	return 0;
}