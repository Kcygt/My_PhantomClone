#define _USE_MATH_DEFINES
#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <826api.h>
using namespace std;

class DAQBoard
{
	public:
		
		int id;
		int openBoard();
		int closeBoard();
		int readEncoder();		
		int readEncoderwithtimestamp();
		double PositionCalc(vector<unsigned int> ENCDAT, int check, vector<double>& data);
		double AngleCalc(unsigned int value, float GearRatio, int check, bool base);
		int forwardKinematic();
		

};
