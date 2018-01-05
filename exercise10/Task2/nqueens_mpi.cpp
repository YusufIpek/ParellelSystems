#include <vector>
#include <iostream>
#include "mpi.h"

using Board = std::vector<int>;

bool is_position_valid(const Board& board, int row, int column) {

	for (int j = 0; j < row; j++) {

		if (board[j] == column) {
			return false;
		}
		if ((board[j] - (row - j)) == column) {
			return false;
		}
		if ((board[j] + (row - j)) == column) {
			return false;
		}

	}

	return true;
}


void board_place_queens(Board& board, int n, int queens, int row, int& solutions) {

	if (queens == 0) {
		solutions++;
		return;
	}


	for (int j = 0; j < n; j++) {
		if (is_position_valid(board, row, j)) {
			board[row] = j;
			board_place_queens(board, n, queens - 1, row + 1, solutions);
		}
	}
}





int main(int argc, char** argv) {

	

	if (argc != 2) {
		std::cout << "Invalid arguments!" << std::endl;
		return 1;
	}

	int n = atoi(argv[1]);

	int myrank, nrOfProcesses;
	int msgtag = 1, solutions = 0, result = 0;
	int tasks, rangeL = 0, rangeU;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nrOfProcesses);

	tasks = n / nrOfProcesses;
	if (myrank < n%nrOfProcesses)
		tasks++;

	//std::cout << "Rank: " << myrank << " tasks: " << tasks << std::endl;

	//compute the lower boundary for each process
	for (int i = 0; i < myrank; i++) {
		if (i < n%nrOfProcesses){
			if (myrank < n%nrOfProcesses)
				rangeL += tasks;
			else
				rangeL += tasks + 1;
		}
		else{
			rangeL += tasks;
		}
	}
	rangeU = rangeL + tasks;
	
	
	auto board = Board(n, 0); //std::vector<int>(n, 0);

	//std::cout << "Rank: " << myrank << " : LowerBoundary: " << rangeL << " UpperBoundary: " << rangeU << std::endl;

	for (int i = rangeL; i < rangeU; i++) {
		board[0] = i;
		board_place_queens(board, n, n-1, 1, solutions);
	}
	MPI_Allreduce(&solutions, &result, 1, MPI_INTEGER, MPI_SUM, MPI_COMM_WORLD);

	if(myrank == 0)
		std::cout << "Solutions: " << result << std::endl;
	MPI_Finalize();
	return 0;
}
