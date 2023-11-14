#include "DAQBoard.h"
#include <iostream>
DAQBoard Board;

int main()
{

	int boardNum = Board.openBoard();
	int check,x;
	cout << "For zero start enter 0 " << endl;
	cin >> check;
	Board.configuration(check, boardNum);
	vector<unsigned int> a, b;
	std::cout << "Running the code" << std::endl;
	// after confiduration
	double x1, y1, z1,x2,y2,z2 = 0.0;
	for (int ii = 0; ii < 100000; ii++) {
		Board.forwardKinematics( check, x1, y1, z1, x2, y2, z2);
		std::cout << "x1  : " << x1 << "  y1   : " << y1 << "  z1   : " << z1 << std::endl;
		//std::cout << "x1:" << x1 << "     x2:" << x2 << std::endl;
	
	}

	int output = Board.closeBoard();
	std::cout << "Closed the board";
	/*
	std::cout << flag << std::endl;
	int output = Board.closeBoard();
	std::cout << output << std::endl;
	void koray();
	*/
	return 0;
}