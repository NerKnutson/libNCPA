#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/xcorrelator.h" // xcorrelator(int,int)
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5 || argc > 6){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --lengthFT <length of Fourier transform> [optional: --humanReadable]" << endl;
		return 1;
	}

	int N_channels = 0;
	int lengthFT = 0;
	bool humanReadable = false;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--lengthFT") == 0 || strcmp(argv[arg], "-lFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
	}

	if(N_channels <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	xcorrelator XCORRELATOR(N_channels, lengthFT);
	unsigned N_correlations = N_channels * (N_channels - 1) / 2;
	// TODO: Maybe fix indexing issues with [0 ... lengthFT][-lengthFT ... -1]
	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(XCORRELATOR.read(std::cin)) {
			if(!XCORRELATOR.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(XCORRELATOR.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(XCORRELATOR.read(stdin)) {
			if(!XCORRELATOR.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(XCORRELATOR.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}

	return 0;
}
