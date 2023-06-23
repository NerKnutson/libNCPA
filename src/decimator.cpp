#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "../include/decimator.h" // decimator(double[],int,int,double,double,double,double[])
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 11){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --decimationFactor <decimation factor> --sampleRate <sample rate (Hz)> --cutoffFrequency <cut off frequency (Hz)> --stopbandRipple <stopband ripple (dB)>"  << endl;
		return 1;
	}
	int N_channel = 0;
	double decimationFactor = 0;
	double sampleRate = 0;
	double cutoffFrequency = 0;
	double stopbandRipple = 0;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--decimationFactor") == 0)
			sscanf(argv[arg + 1], "%lf", &decimationFactor);
		else if(strcmp(argv[arg], "--sampleRate") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleRate);
		else if(strcmp(argv[arg], "--cutoffFrequency") == 0)
			sscanf(argv[arg + 1], "%lf", &cutoffFrequency);
		else if(strcmp(argv[arg], "--stopbandRipple") == 0)
			sscanf(argv[arg + 1], "%lf", &stopbandRipple);
	}

	if(2*cutoffFrequency > sampleRate) {
		cerr << "Given cutoff frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	}

	if(N_channel < 0) {
		cerr << "Error in " << argv[0] << ":\nInvalid number of channels" << endl;
		return 1;
	}

	int indexData = 0;
	double inputBuffer[N_channel];
	decimator DECIMATOR(N_channel, decimationFactor, sampleRate, cutoffFrequency, stopbandRipple);
	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(cin >> inputBuffer[indexData]) {
		indexData++;
		if(indexData % N_channel == 0) {
			indexData = 0;
			double* outputDecimator = DECIMATOR.decimate(inputBuffer);
			if(outputDecimator != NULL) {
				for(int c = 0; c < N_channel; ++c) {
					if(c < N_channel - 1)
						cout << outputDecimator[c] << "\t";
					else
						cout << outputDecimator[c] << endl;
				}
			} else
				continue;
		}
	}

	return 0;
}
