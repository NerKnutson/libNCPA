#define MAX_CHANNELS 128

// Note: your units should be in the KMS standard (kilogram, meter, second)
#define ARRAY_GEOMETRY_FMT "<x1>\t<y1>\t<z1>\n" \
                           "<x2>\t<y2>\t<z2>\n" \
                           "...\n"
#define SIGNAL_SOURCE_FMT "sampleFrequency = <sample frequency>\n"\
	                       "speed = <signal speed in meters per second>\n"\
	                       "azimuth = <azimuthal angle of signal direction in degrees>\n"\
	                       "inclination = <polar angle of signal direction in degrees>\n"\

#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setw(int)
#include <random> // std::default_random_engine generator std::normal_distribution<double> dist(double, double)
#include <climits> // INT_MAX
#include <cstring> // std::strcmp(char*, char*)
#include <filesystem> // std::filesystem::path
#include "../include/importArrayGeometry.h"
#ifndef inputBufferSize
#define inputBufferSize 8192
#endif
using namespace std;
double calcSlownessVector(FILE* fileName, double slowness[3]);
void calcSampleDelays(double sampleDelays[], double arrayGeometry[][3], double slowness[3], unsigned N_channel, double sampleFrequency);

int main(int argc, char* argv[]) {
	if(argc != 5){
		cerr << "Usage: " << argv[0] << " --arrayGeometry <array geometry filename> --signalInfo <signal information filename>\n" << endl;
		cerr << "Array Geometry File Format:\n"<< ARRAY_GEOMETRY_FMT << endl;
		cerr << "Signal Source File Format:\n"<< SIGNAL_SOURCE_FMT << endl;
		cerr << "Note: units should follow the KMS standard (kilogram, meter, second)\n" << endl;
		return 1;
	}

	double arrayGeometry[MAX_CHANNELS][3];
	int N_channel = 0;
	FILE* inputFile = NULL;
	filesystem::path path;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--arrayGeometry") == 0)
			N_channel = importArrayGeometry(argv[arg + 1], arrayGeometry);
		else if(strcmp(argv[arg], "--signalInfo") == 0) {
			inputFile = fopen(argv[arg + 1], "r");
			path = argv[arg + 1];
		}
	}
	if(N_channel <= 0 || inputFile == NULL) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	double slowness[3];
	double sampleFrequency = calcSlownessVector(inputFile, slowness);


	double sampleDelays[N_channel];
	calcSampleDelays(sampleDelays, arrayGeometry, slowness, N_channel, sampleFrequency);

	double inputBuffer[inputBufferSize];
	fill(inputBuffer, inputBuffer+inputBufferSize, 0.0);
	unsigned inputIndex = 0;

	cout << setprecision(numeric_limits<double>::digits10 + 1);
	filesystem::file_time_type fileTime = filesystem::last_write_time(path);
	while(cin >> inputBuffer[inputIndex]) {
		for(int c = 0; c < N_channel; ++c) {
			int upperDelay = std::ceil(sampleDelays[c]);
			int lowerDelay = std::floor(sampleDelays[c]);
			unsigned channelInputIndexUpper = (inputIndex + upperDelay + inputBufferSize) % inputBufferSize;
			unsigned channelInputIndexLower = (inputIndex + lowerDelay + inputBufferSize) % inputBufferSize;
			double output = 0;
			if(upperDelay == lowerDelay)
				output = inputBuffer[channelInputIndexUpper];
			else
				output = (upperDelay - sampleDelays[c]) * inputBuffer[channelInputIndexUpper] + (sampleDelays[c] - lowerDelay) * inputBuffer[channelInputIndexLower];
			if(c < N_channel - 1)
				cout << output << "\t";
			else
				cout << output << endl;
		}
		inputIndex = (inputIndex + 1) % inputBufferSize;
		filesystem::file_time_type checkFileTime = filesystem::last_write_time(path);
		if(fileTime != checkFileTime) {
			sampleFrequency = calcSlownessVector(inputFile, slowness);
			calcSampleDelays(sampleDelays, arrayGeometry, slowness, N_channel, sampleFrequency);
			fileTime = checkFileTime;
		}
	}

	fclose(inputFile);
	return 0;
}

// returns sample frequency
double calcSlownessVector(FILE* fileName, double slowness[3]) {
	fflush(fileName);
	rewind(fileName);
	double sampleFrequency;
	double slownessCheck = 0;
	if(fileName == NULL) {
		cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal source filename" << endl;
		return 1;
	} else {
		fscanf(fileName, "sampleFrequency = %lf\n", &sampleFrequency);
		if(sampleFrequency <= 0.0) {
			cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal sample frequency" << endl;
			return 1;
		}
		double speed = 0.0;
		fscanf(fileName, "speed = %lf\n", &speed);
		if(speed <= 0.0) {
			cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal speed" << endl;
			return 1;
		}
		double azimuth = 0.0;
		double inclination = 0.0;
		fscanf(fileName, "azimuth = %lf\n", &azimuth);
		fscanf(fileName, "inclination = %lf\n", &inclination);

		azimuth *= M_PI/180.0;
		inclination *= M_PI/180.0;
		slowness[0] = cos(azimuth) * cos(inclination) / speed;
		slowness[1] = sin(azimuth) * cos(inclination) / speed;
		slowness[2] = sin(inclination) / speed;
		for(int d = 0 ; d < 3; ++d) {
			slownessCheck += slowness[d] * slowness[d];
		}
		if(slownessCheck <= 0.0) {
			cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal direction" << endl;
			return 1;
		}
	}
	return sampleFrequency;
}
void calcSampleDelays(double sampleDelays[], double arrayGeometry[][3], double slowness[3], unsigned N_channel, double sampleFrequency) {
	int maxDelay = -INT_MAX;
	int minDelay = INT_MAX;
	for(int c = 0; c < N_channel; ++c) {
		sampleDelays[c] = 0;
		for(int d = 0 ; d < 3; ++d) {
			sampleDelays[c] -= arrayGeometry[c][d] * slowness[d] * sampleFrequency;
		}
		if(sampleDelays[c] > maxDelay)
			maxDelay = sampleDelays[c];
		if(sampleDelays[c] < minDelay)
			minDelay = sampleDelays[c];
	}
	for(int c = 0; c < N_channel; ++c) {
		sampleDelays[c] += abs(minDelay) + 1;
	}
}
