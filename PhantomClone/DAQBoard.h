#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include "826api.h"
class DAQBoard
{
	public:
		int id;
		int openBoard();
		int closeBoard();
		int readEncoder();
};
