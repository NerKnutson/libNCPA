#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/decimator.h" // decimator(int,int,double,double,double)
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 11 && argc !=12){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --decimationFactor <decimation factor> --sampleFrequency <sample frequency (Hz)> --cutoffFrequency <cut off frequency (Hz)> --stopbandRipple <stopband ripple (dB)> [optional: --humanReadable]"  << endl;
		return 1;
	}
	int N_channels = 0;
	double decimationFactor = 0;
	double sampleFrequency = 0;
	double cutoffFrequency = 0;
	double stopbandRipple = 0;
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--decimationFactor") == 0 || strcmp(argv[arg], "-dF") == 0)
			sscanf(argv[arg + 1], "%lf", &decimationFactor);
		else if(strcmp(argv[arg], "--sampleFrequency") == 0 || strcmp(argv[arg], "-sF") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleFrequency);
		else if(strcmp(argv[arg], "--cutoffFrequency") == 0 || strcmp(argv[arg], "-cF") == 0)
			sscanf(argv[arg + 1], "%lf", &cutoffFrequency);
		else if(strcmp(argv[arg], "--stopbandRipple") == 0 || strcmp(argv[arg], "-sR") == 0)
			sscanf(argv[arg + 1], "%lf", &stopbandRipple);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
	}

	if(2*cutoffFrequency > sampleFrequency) {
		cerr << "Given cutoff frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	}

	if(N_channels < 0) {
		cerr << "Error in " << argv[0] << ":\nInvalid number of channels" << endl;
		return 1;
	}

	decimator<double, double> DECIMATOR(N_channels, decimationFactor, sampleFrequency, cutoffFrequency, stopbandRipple);

	if(humanReadable) { // Print human-readable data
		while(DECIMATOR.read(std::cin)){
			if(!DECIMATOR.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(DECIMATOR.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(DECIMATOR.read(stdin)){
			if(!DECIMATOR.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(DECIMATOR.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}

	return 0;
}
