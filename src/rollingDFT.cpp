#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <vector> // std::vector<>
#include "transformers/rollingDFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --lengthFT <length of Fourier Transform> [optional: --humanReadable] [optional: <bin indices>]"  << endl;
		return 1;
	}
	unsigned N_channels = 0;
	unsigned lengthFT = 0;
	unsigned N_bins = argc - 5;
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--lengthFT") == 0 || strcmp(argv[arg], "-lFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0) {
			humanReadable = true;
			N_bins = argc - 6;
		}
	}
	if(N_channels <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	unsigned indexBins[lengthFT/2 + 1];
	if(N_bins > 0) {
		for(int arg = argc - N_bins; arg < argc; ++arg) {
			int b = arg - argc + N_bins;
			indexBins[b] = stoi(argv[arg]);
			if(indexBins[b] < 0 || 2 * indexBins[b] > lengthFT) {
				cerr << "Error in " << argv[0] << ":\nInvalid bin index" << endl;
				return 1;
			}
		}
	} else {
		N_bins = lengthFT/2 + 1;
		for(int b = 0; b < N_bins; ++b)
			indexBins[b] = b;
	}
	rollingDFT<double, std::complex<double>> roller(N_channels, lengthFT, std::span(indexBins, N_bins));

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(roller.read(std::cin) > 0) {
			if(!roller.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(roller.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(roller.read(stdin) > 0) {
			if(!roller.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(roller.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}
	return 0;
}
