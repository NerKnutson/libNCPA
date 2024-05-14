#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/fastFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5 || argc > 6){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --lengthFT <length of Fourier Transform> [optional: --humanReadable]"  << endl;
		return 1;
	}

	unsigned N_channels = 0;
	unsigned lengthFT = 0;
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

	fastFT<double, std::complex<double>> r2cFFT(N_channels, lengthFT);

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(r2cFFT.read(std::cin) > 0) {
			if(!r2cFFT.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(r2cFFT.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(r2cFFT.read(stdin) > 0) {
			if(!r2cFFT.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(r2cFFT.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}

	return 0;
}
