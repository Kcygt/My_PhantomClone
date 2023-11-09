#include "DAQBoard.h"
#include <iostream>
DAQBoard Board;

int main() {
	int boardNum = Board.openBoard();
	int check;
	cout << "For zero start enter 0 " << endl;
	cin >> check;
	Board.configuration(check, boardNum);
/*
	Board.readEncoder();
	std::cout << flag << std::endl;
	int output = Board.closeBoard();
	std::cout << output << std::endl;
	void koray();
	*/
}