#include <vector>
#include <iostream>


using Board = std::vector<int>;

bool is_position_valid(const Board& board, int row, int column){
	
	
	
		for(int j = 0; j < row; j++){
			
				if(board[j]==column){
					return false;
				}
				if((board[j]-(row - j)) == column){
					return false;
				}
				if((board[j]+(row - j)) == column){
					return false;
				}	
					
		}
	
	return true;
}


void board_place_queens(Board board, int n, int queens, int row, int& solutions){
	
	if(queens == 0){
    #pragma omp atomic
	  solutions++;
		return;
	}
	
  #pragma omp for schedule(dynamic)
	for(int j = 0; j < n; j++){
		if(is_position_valid(board, row, j)){
			board[row] = j;
			board_place_queens(board, n, queens-1, row+1, solutions);
		}
	}
}


 


int main(int argc, char** argv){
	
	if(argc != 2){
		std::cout << "Invalid arguments!" << std::endl;
		return 1;
	}
	
	int n = atoi(argv[1]);
	int solutions = 0;
	
  #pragma omp parallel
  {
  	auto board = Board(n,0); //std::vector<int>(n, 0);
  	
  	board_place_queens(board, n, n, 0, solutions);
  }
	std::cout << "Solutions: " << solutions << std::endl;
	
	return 0;
}








