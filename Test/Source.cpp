#include "DAQBoard.h"
#include <iostream>
DAQBoard Board;

int main() {
	int flag = Board.openBoard();
	Board.readEncoder();
	std::cout << flag << std::endl;
	int output = Board.closeBoard();
	std::cout << output << std::endl;
}