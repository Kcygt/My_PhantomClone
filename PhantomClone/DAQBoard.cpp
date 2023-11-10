#include "pch.h"
#include "DAQBoard.h"
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <826api.h>
using namespace std;
#include <vector>


DAQBoard Board1;
int DAQBoard::openBoard() {
	int id, flags = S826_SystemOpen();
	uint boardNum=0;
	if (flags < 0)
		cout << "S826_SystemOpen returned error code " << flags << endl;
	else if (flags == 0)
		cout << "No boards were detected" << endl;
	else {
		cout << "Boards were detected with these IDs: ";
		for (id = 0; id < 16; id++) {
			if (flags & (1 << id))
			{
				cout << id << " " << endl;
				boardNum = id;
			}
		}
	
	}
	cout << "Using board: " << boardNum << endl;
	return boardNum;

}

int DAQBoard::closeBoard() {
	int output = S826_SystemClose();
	if (output == 0)
		std::cout << "Closed the board" << std::endl;
	else
		std::cout << "The board not closed !!!" << std::endl;
	return output;
}

int DAQBoard::configuration(int check, uint boardNum)
{
	if (check == 0) {
		// Configure encoder interface
		for (uint ch = 0; ch < 6; ++ch)
		{
			S826_CounterStateWrite(boardNum, ch, 0);							// Stop channel operation
			S826_CounterModeWrite(boardNum, ch, S826_CM_K_QUADX4);			// Configure counter
			S826_CounterStateWrite(boardNum, ch, 1);							// Start channel operation
			S826_CounterPreloadWrite(boardNum, ch, 0, 0);						// Set preload register
			S826_CounterPreload(boardNum, ch, 1, 0);							// Copy preload value to counter
		}
	}
	else {
		// Configure encoder interface
		for (uint ch = 0; ch < 6; ++ch)
		{
			S826_CounterStateWrite(boardNum, ch, 0);							// Stop channel operation
			S826_CounterModeWrite(boardNum, ch, S826_CM_K_QUADX4);			// Configure counter
			S826_CounterStateWrite(boardNum, ch, 1);							// Start channel operation
			S826_CounterPreloadWrite(boardNum, ch, 0, 100000);						// Set preload register
			S826_CounterPreload(boardNum, ch, 1, 0);							// Copy preload value to counter
		}

	}

	
	// Configure analog output
	for (uint ch = 0; ch < 7; ++ch)
	{
		S826_DacRangeWrite(boardNum, ch, S826_DAC_SPAN_10_10, 0);
		//SetDacOutput(boardNum, ch, S826_DAC_SPAN_5_5, 0);
	}

	// Set output to 0V
	for (uint ch = 0; ch < 7; ++ch)
	{
		uint setpoint;
		double volts = 0;
		// conversion is based on dac output range:
		switch (S826_DAC_SPAN_10_10)
		{
		case S826_DAC_SPAN_0_5:							// 0 to +5V
			setpoint = (uint)(volts * 0xFFFF / 5);
			break;
		case S826_DAC_SPAN_0_10:						// 0 to +10V
			setpoint = (uint)(volts * 0xFFFF / 10);
			break;
		case S826_DAC_SPAN_5_5:							// -5V to +5V
			setpoint = (uint)(volts * 0xFFFF / 10) + 0x8000;
			break;
		case S826_DAC_SPAN_10_10:						// -10V to +10V
			setpoint = (uint)(volts * 0xFFFF / 20) + 0x8000;
			break;
		}

		S826_DacDataWrite(boardNum, ch, setpoint, 0);  // program DAC output
		//SetDacOutput(boardNum, ch, S826_DAC_SPAN_10_10, 0);
		//SetDacOutput(boardNum, ch, S826_DAC_SPAN_5_5, 0);
	}
	std::cout << "Completed configuration" << std::endl;
	return 0;
}

int DAQBoard::readEncoder(vector<unsigned int>& ENCDAT1, vector<unsigned int>& ENCDAT2) {
	// Configure encoder interface#

	int boardNum = 0;
	uint counts;
	
	for (uint ch = 0; ch < 6; ++ch)
	{
		S826_CounterRead(boardNum, ch, &counts);          // Fetch the current counts from counter 0.
		if (ch <=2 )
			ENCDAT1.push_back(counts);
		else
			ENCDAT2.push_back(counts);
	}
	return 0;
}

int DAQBoard::readEncoderwithtimestamp(vector<unsigned int>& ENCDAT1, vector<unsigned int>& ENCDAT2) {
	int boardNum = 0;
	unsigned int counts[3] = { 0, 0, 0 };
	unsigned int ctstamp = 0;
	unsigned int reason = 0;
		for (unsigned int channel = 3; channel < 6; channel++)
		{
			S826_CounterSnapshot(boardNum, channel); // Create Snapshot
			S826_CounterSnapshotRead(boardNum, channel - 3, &counts[channel - 3], &ctstamp, &reason, 0); // Read from snapshot
			ENCDAT1.push_back(counts[channel - 3]);
			S826_CounterSnapshotRead(boardNum, channel, &counts[channel - 3], &ctstamp, &reason, 0); // Read from snapshot
			ENCDAT2.push_back(counts[channel - 3]);

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

double DAQBoard::PositionCalc(vector<unsigned int> ENCDAT, int check)
{
	float gearRatio[3];
	double Position[3];
	double x, y, z;
	double angles[3];
	double sin[4];
	double cos[4];

	const float length1 = 0.203f; // extra link lentgh (along z axis)
	const float length2 = 0.215f; // parallel linkage length
	const float length3 = 0.203f; // end effector linkage
	const float phantomHeightDiff = 0.087f;
	//	const float phantomYDisplacement = 0.035f; //added to lengths 1 & 2 so now obsolete (is thimble length)

	//bools to crontol scalar values used for simulsolve method
	bool base = true;
	bool arm = false;

	gearRatio[0] = 13.4f;
	gearRatio[1] = 11.5f;
	gearRatio[2] = 11.5f;

	//modified angle calc from Guy's chai code
	angles[0] = AngleCalc(ENCDAT[2], gearRatio[0], check, base);// Calculate angles from encoder values
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
	x = Position[0];
	y = Position[1];
	z = Position[2];
	if (check == 0)
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
	}

	x = 0.0;
	y = 0.0;
	z = 0.0;
	return 0;
}

double DAQBoard::forwardKinematics( int check, double& x1, double& y1, double& z1, double& x2, double& y2, double& z2)
{
	
	vector<unsigned int> ENCDAT1;
	vector<unsigned int> ENCDAT2;
	ENCDAT1.clear();
	ENCDAT2.clear();
	Board1.readEncoder(ENCDAT1, ENCDAT2);
	float gearRatio[3];
	double Position[3];
	double angles1[3];
	double sin1[4];
	double cos1[4];
	
	double angles2[3];
	double sin2[4];
	double cos2[4];


	const float length1 = 0.203f; // extra link lentgh (along z axis)
	const float length2 = 0.215f; // parallel linkage length
	const float length3 = 0.203f; // end effector linkage
	const float phantomHeightDiff = 0.087f;
	//	const float phantomYDisplacement = 0.035f; //added to lengths 1 & 2 so now obsolete (is thimble length)

	//bools to crontol scalar values used for simulsolve method
	bool base = true;
	bool arm = false;

	gearRatio[0] = 13.4f;
	gearRatio[1] = 11.5f;
	gearRatio[2] = 11.5f;

	//modified angle calc from Guy's chai code
	angles1[0] = AngleCalc(ENCDAT1[2], gearRatio[0], check, base);// Calculate angles from encoder values
	angles1[1] = AngleCalc(ENCDAT1[0], gearRatio[1], check, arm); // Calculate angles from encoder values
	angles1[2] = AngleCalc(ENCDAT1[1], gearRatio[2], check, arm); // Calculate angles from encoder values 

	
	angles2[0] = AngleCalc(ENCDAT2[2], gearRatio[0], check, base);// Calculate angles from encoder values
	angles2[1] = AngleCalc(ENCDAT2[0], gearRatio[1], check, arm); // Calculate angles from encoder values
	angles2[2] = AngleCalc(ENCDAT2[1], gearRatio[2], check, arm); // Calculate angles from encoder values 

	// Pre Calculate Trig
	sin1[1] = std::sin(angles1[0]);
	sin1[2] = std::sin(angles1[1]);
	sin1[3] = std::sin(angles1[2]);
	cos1[1] = std::cos(angles1[0]);
	cos1[2] = std::cos(angles1[1]);
	cos1[3] = std::cos(angles1[2]);

	// Pre Calculate Trig
	sin2[1] = std::sin(angles2[0]);
	sin2[2] = std::sin(angles2[1]);
	sin2[3] = std::sin(angles2[2]);
	cos2[1] = std::cos(angles2[0]);
	cos2[2] = std::cos(angles2[1]);
	cos2[3] = std::cos(angles2[2]);
	// Calculate Position
	x1 = (cos1[1] * ((length2 * cos1[2]) + (length3 * sin1[3])) - length2);
	y1 = sin1[1] * ((length2 * cos1[2]) + (length3 * sin1[3]));
	z1 = length1 - (length3 * cos1[3]) + (length2 * sin1[2]);

	x2 = (cos2[1] * ((length2 * cos2[2]) + (length3 * sin2[3])) - length2);
	y2 = sin2[1] * ((length2 * cos2[2]) + (length3 * sin2[3]));
	z2 = length1 - (length3 * cos2[3]) + (length2 * sin2[2]);

	//rotate co-ord frame to desired

	
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

	return 0.0;
}

std::vector<std::vector<double>> DAQBoard::Jacobian(vector<unsigned int> encoderValue, float gearRatio[3],  int check)
{
	double sin[4];
	double cos[4];
	bool base = true;
	bool arm = false;
	const float length1 = 0.2175f; //0.168 without thimble
	const float length2 = 0.215f;
	const float length3 = 0.2175f; //0.168 without thimble
	const float phantomHeightDiff = 0.087;
	const float phantomYDisplacement = 0.035f; // (115.0f/1000.0f)/2.0f
	double angles[3];

	angles[0] = AngleCalc(encoderValue[2], gearRatio[0], check, base);// Calculate angles from encoder values
	angles[1] = AngleCalc(encoderValue[0], gearRatio[1], check, arm); // Calculate angles from encoder values
	angles[2] = AngleCalc(encoderValue[1], gearRatio[2], check, arm); // Calculate angles from encoder values 

	sin[1] = std::sin(angles[0]);
	sin[2] = std::sin(angles[1]);
	sin[3] = std::sin(angles[2]);
	cos[1] = std::cos(angles[0]);
	cos[2] = std::cos(angles[1]);
	cos[3] = std::cos(angles[2]);

	std::vector<std::vector<double>> J;
	J.push_back({ sin[1] * ((length2 * cos[2]) + (length3 * sin[3])), -length2 * cos[1] * sin[1], length3 * cos[1] * sin[3] });
	J.push_back({ cos[1] * ((length2 * cos[2]) + (length3 * sin[3])), -length2 * sin[1] * sin[2], length3 * sin[1] * cos[3] });
	J.push_back({ 0, length2 * cos[2], length3 * sin[3] });
	
	return J;
}



int DAQBoard::calibration(uint boardNum, int channel)
{
	vector<unsigned int> ENCDAT1; //variable for storing the encoder data
	vector<unsigned int> ENCDAT2; //variable for storing the encoder data


	return 0;
}


