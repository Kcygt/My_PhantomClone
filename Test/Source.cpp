#include "DAQBoard.h"
#include <iostream>
int main() {

	DAQBoard Board;
	int flag = Board.openBoard();
	//int flag = S826_SystemOpen();
	Board.readEncoder();
	std::cout << flag << std::endl;
	int output = Board.closeBoard();
	std::cout << output << std::endl;
}