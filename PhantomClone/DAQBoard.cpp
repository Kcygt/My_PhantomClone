#include "pch.h"
#include "DAQBoard.h"
#include <iostream>
using namespace std;
int DAQBoard::openBoard() {
	int flag = S826_SystemOpen();
	cout << "Opened the board" << endl;
	return flag;
}


int DAQBoard::closeBoard() {
	int output = S826_SystemClose();
	cout << "Closed the board" << endl;

	return output;
}



int DAQBoard::readEncoder() {
	// Configure encoder interface#

	int boardNum = 0;
	uint counts;
	int a;

	for (uint ch = 0; ch < 6; ++ch)
	{
		S826_CounterRead(boardNum, ch, &counts);          // Fetch the current counts from counter 0.
		std::cout << "Channel " << ch << ":" << counts << std::endl;
	}
	
	return 0;
}

