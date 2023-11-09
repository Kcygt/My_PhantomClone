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
		int configuration(int check, uint boardNum);
		int readEncoder(vector<unsigned int>& ENCDAT1, vector<unsigned int>& ENCDAT2);
		int readEncoderwithtimestamp(vector<unsigned int>& ENCDAT1, vector<unsigned int>& ENCDAT2);
		double AngleCalc(unsigned int value, float GearRatio, int check, bool base);
		double PositionCalc(vector<unsigned int> ENCDAT, int check);		
		double forwardKinematics(vector<unsigned int> ENCDAT, int check, double& x, double& y, double& z);
		std::vector<std::vector<double>> Jacobian(vector<unsigned int> ENCDAT, float gearRatio[3],  int check);
		
		
		int calibration(uint boardNum, int channel);

};
