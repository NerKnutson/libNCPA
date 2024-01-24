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
	unsigned N_sample = argc - 5;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--lengthFT") == 0 || strcmp(argv[arg], "-lFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0) {
			N_sample = argc - 6;
			humanReadable = true;
		}
	}
	if(N_channel <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	unsigned indexSample[N_sample];
	for(int arg = argc - N_sample; arg < argc; ++arg) {
		int b = arg - argc + N_sample;
		indexSample[b] = stoi(argv[arg]);
		if(indexSample[b] < 0 || 2 * indexSample[b] > lengthFT) {
			cerr << "Error in " << argv[0] << ":\nInvalid sample index" << endl;
			return 1;
		}
	}

	fastFT<std::complex<double>> c2rFFT(N_channel, lengthFT);
	unsigned indexData = 0;

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(cin >> c2rFFT.complexData[indexData]) {
			indexData++;
			if(indexData == N_channel*(lengthFT/2 + 1)) {
				indexData = 0;
				c2rFFT.FFT();
				if(N_sample > 0) {
					for(int s = 0; s < N_sample; ++s) {
						for(int c = 0; c < N_channel; ++c) {
							cout << c2rFFT.realData[c + N_channel*indexSample[s]];;
							if(c < N_channel - 1)
								cout << "\t";
							else
								cout << endl;
						}
					}
				} else { // Display all samples
					for(int s = 0; s < lengthFT; ++s) {
						for(int c = 0; c < N_channel; ++c) {
							cout << c2rFFT.realData[c + N_channel*s];
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
		unsigned sizeInput = N_channel*(lengthFT/2 + 1);
		unsigned sizeOutput = N_channel*lengthFT;
		if(N_sample > 0) {
			while(fread(c2rFFT.realData, sizeInput, sizeof(std::complex<double>), stdin)) {
				c2rFFT.FFT();
				for(int s = 0; s < N_sample; ++s)
					fwrite(&c2rFFT.realData[N_channel*indexSample[s]], N_channel, sizeof(double), stdout);
			}
		} else {
			while(fread(c2rFFT.realData, sizeInput, sizeof(std::complex<double>), stdin)) {
				c2rFFT.FFT();
				fwrite(c2rFFT.realData, sizeOutput, sizeof(double), stdout);
			}
		}
	}

	return 0;
}
