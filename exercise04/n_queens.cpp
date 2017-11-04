#include <vector>
#include <iostream>


using Board = std::vector<std::vector<int>>;


Board initialize_board(int n){
	
	Board board(n);
	for(int i = 0; i < n; i++){
		board[i].resize(n);
		for(int j = 0; j < n; j++){
			board[i][j] = 0;
		}
	}
	return board;
}


bool is_position_valid(const Board& board, int row, int column, int n){
	
	for(int i = 0; i < row; i++){
		for(int j = 0; j < n; j++){
			if(board[i][j] == 1){
				if(j==column){
					return false;
				}
				if((j-(row - i)) == column){
					return false;
				}
				if((j+(row - i)) == column){
					return false;
				}	
			}		
		}
	}
	return true;
}


void board_place_queens(Board board, int n, int queens, int row, int& solutions){
	
	if(queens == 0){
		solutions++;
		return;
	}
	

	for(int j = 0; j < n; j++){
		if(is_position_valid(board, row, j, n)){
			board[row][j] = 1;
			board_place_queens(board, n, queens-1, row+1, solutions);
			board[row][j] = 0;
		}
	}
}


 


int main(int argc, char** argv){
	
	if(argc != 2){
		std::cout << "Invalid arguments!" << std::endl;
		return 1;
	}
	
	int n = atoi(argv[1]);
	
	
	auto board = initialize_board(n);
	int solutions = 0;
	board_place_queens(board, n, n, 0, solutions);

	std::cout << "Solutions: " << solutions << std::endl;
	
	return 0;
}








