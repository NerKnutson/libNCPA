#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "../include/fastFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --lengthFT <length of Fourier Transform> [optional: --humanReadable] [optional: <bin indices>]"  << endl;
		return 1;
	}

	unsigned N_channel = 0;
	unsigned lengthFT = 0;
	bool humanReadable = false;
	unsigned N_bin = argc - 5;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--lengthFT") == 0 || strcmp(argv[arg], "-lFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0) {
			N_bin = argc - 6;
			humanReadable = true;
		}
	}
	if(N_channel <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	unsigned indexBin[N_bin];
	for(int arg = argc - N_bin; arg < argc; ++arg) {
		int b = arg - argc + N_bin;
		indexBin[b] = stoi(argv[arg]);
		if(indexBin[b] < 0 || 2 * indexBin[b] > lengthFT) {
			cerr << "Error in " << argv[0] << ":\nInvalid bin index" << endl;
			return 1;
		}
	}

	fastFT<double> r2cFFT(N_channel, lengthFT);
	unsigned indexData = 0;

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(cin >> r2cFFT.realData[indexData]) {
			indexData++;
			if(indexData == N_channel*lengthFT) {
				indexData = 0;
				r2cFFT.FFT();
				if(N_bin > 0) {
					for(int b = 0; b < N_bin; ++b) {
						for(int c = 0; c < N_channel; ++c) {
							cout << r2cFFT.complexData[c + N_channel*indexBin[b]];;
							if(c < N_channel - 1)
								cout << "\t";
							else
								cout << endl;
						}
					}
				} else { // Display all frequency bins
					for(int f = 0; f < lengthFT/2 + 1; ++f) {
						for(int c = 0; c < N_channel; ++c) {
							cout << r2cFFT.complexData[c + N_channel*f];
							if(c < N_channel - 1)
								cout << "\t";
							else
								cout << endl;
						}
					}
				}
			}
		}
	} else { // Print binary representation
		unsigned sizeInput = N_channel*lengthFT;
		unsigned sizeOutput = N_channel*(lengthFT/2 + 1);
		if(N_bin > 0) {
			while(fread(r2cFFT.realData, sizeInput, sizeof(double), stdin)) {
				r2cFFT.FFT();
				for(int b = 0; b < N_bin; ++b)
					fwrite(&r2cFFT.complexData[N_channel*indexBin[b]], sizeOutput, sizeof(std::complex<double>), stdout);
			}
		} else {
			while(fread(r2cFFT.realData, sizeInput, sizeof(double), stdin)) {
				r2cFFT.FFT();
				fwrite(r2cFFT.complexData, sizeOutput, sizeof(std::complex<double>), stdout);
			}
		}
	}

	return 0;
}
