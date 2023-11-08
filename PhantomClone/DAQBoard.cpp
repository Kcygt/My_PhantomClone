#include "pch.h"
#include "DAQBoard.h"
#include <iostream>
#include <vector>
using namespace std;



int DAQBoard::openBoard() {
	int flag = S826_SystemOpen();
	std::cout << "Opened the board" << std::endl;
	return flag;
}


int DAQBoard::closeBoard() {
	int output = S826_SystemClose();
	std::cout << "Closed the board" << std::endl;

	return output;
}



int DAQBoard::readEncoder() {
	// Configure encoder interface#

	int boardNum = 0;
	uint counts;

	for (uint ch = 0; ch < 6; ++ch)
	{
		S826_CounterRead(boardNum, ch, &counts);          // Fetch the current counts from counter 0.
		std::cout << "Channel " << ch << ":" << counts << std::endl;
	}
	
	return 0;
}
int DAQBoard::readEncoderwithtimestamp() {
	int boardNum = 0;
	unsigned int ctstamp = 0;
	unsigned int reason = 0;
	unsigned int encoderValue[3] = { 0,0,0 };
	for (int channel = 0; channel < 3; channel++) {
		S826_CounterSnapshot(boardNum, channel); // Create Snapshot
		S826_CounterSnapshotRead(boardNum, channel, &encoderValue[channel], &ctstamp, &reason, 0); // Read from snapshot
	}
	return 0;
}


double DAQBoard::AngleCalc(unsigned int value, float GearRatio, int check, bool base)
{
	const unsigned int maxData = 0xFFFFFFFF;
	double outputValue = 0;

	//from simulove for alt angle test
	double K1 = 0.000128416;
	double K2 = 0.000140087;

	if (check == 1)
	{
		value -= 100000;
	}
	if (value >= (maxData / 2))
	{
		outputValue = (double)value - (double)maxData;
	}
	else
	{
		outputValue = value;
	}

	//// angle calc from Guy's chai code
	//outputValue *= 2 * M_PI;
	//outputValue /= 4000;
	//outputValue /= GearRatio;

	if (base == true) {
		//calc from simusolve
		outputValue = outputValue * K1;
	}
	else {
		//calc from simusolve
		outputValue = outputValue * K2;
	}


	return outputValue;

}

double PositionCalc(vector<unsigned int> ENCDAT, int check, vector<double>& data)
{
	float gearRatio[3] = {0.0, 0.0, 0.0};
	double Position[3] = { 0.0, 0.0, 0.0 };
	double x, y, z;
	double angles[3] = { 0.0, 0.0, 0.0 };
	double sin[4] = { 0.0, 0.0, 0.0, 0.0 };
	double cos[4] = { 0.0, 0.0, 0.0, 0.0 };

	const float length1 = 0.203f; // extra link lentgh (along z axis)
	const float length2 = 0.215f; // parallel linkage length
	const float length3 = 0.203f; // end effector linkage
	const float phantomHeightDiff = 0.087f;
	//	const float phantomYDisplacement = 0.035f; //added to lengths 1 & 2 so now obsolete (is thimble length)

	string msg, msg2;
	stringstream strg, strg2;

	//bools to crontol scalar values used for simulsolve method
	bool base = true;
	bool arm = false;

	gearRatio[0] = 13.4f;
	gearRatio[1] = 11.5f;
	gearRatio[2] = 11.5f;

	//modified angle calc from Guy's chai code
	angles[0] = DAQBoard::AngleCalc(ENCDAT[2], gearRatio[0], check, base);// Calculate angles from encoder values
	angles[1] = AngleCalc(ENCDAT[0], gearRatio[1], check, arm); // Calculate angles from encoder values
	angles[2] = AngleCalc(ENCDAT[1], gearRatio[2], check, arm); // Calculate angles from encoder values 

	
	// Pre Calculate Trig
	sin[1] = std::sin(angles[0]);
	sin[2] = std::sin(angles[1]);
	sin[3] = std::sin(angles[2]);
	cos[1] = std::cos(angles[0]);
	cos[2] = std::cos(angles[1]);
	cos[3] = std::cos(angles[2]);

	// Calculate Position
	x = (cos[1] * ((length2 * cos[2]) + (length3 * sin[3])) - length2);
	y = sin[1] * ((length2 * cos[2]) + (length3 * sin[3]));
	z = length1 - (length3 * cos[3]) + (length2 * sin[2]);

	//rotate co-ord frame to desired

	Position[0] = x;
	Position[1] = y;
	Position[2] = z;

	/*if (check == 0)
	{
		Position[0] = x;
		Position[1] = -y;
		Position[2] = -z;
	}
	else if (check == 1)
	{
		Position[0] = -x;
		Position[1] = y;
		Position[2] = -z;
	}*/

	
	x = 0.0;
	y = 0.0;
	z = 0.0;
	return 0;
}

int DAQBoard::forwardKinematic() {
	const float length1 = 0.2175f; // 0.168 without thimble
	const float length2 = 0.215f; 
	const float length3 = 0.2175f; // 0.168 without thimble
	const float phantomHeightDiff = 0.087f;
	const float phantomYDisplacement = 0.035f; // (115.0f/1000.0f)/2.0f
	return 0;
}

