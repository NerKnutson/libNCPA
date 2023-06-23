#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "../include/fastFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 5){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --lengthFT <length of Fourier Transform>"  << endl;
		return 1;
	}
	unsigned N_channel = 0;
	unsigned lengthFT = 0;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--lengthFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
	}
	if(N_channel <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	fastFT<double, std::complex<double>> r2cFFT(N_channel, lengthFT);
	unsigned indexData = 0;

	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(cin >> r2cFFT.rawInput[indexData]) {
		indexData++;
		if(indexData == N_channel*lengthFT) {
			r2cFFT.FFT();
			for(int f = 0; f < lengthFT/2 + 1; ++f) {
				for(int c = 0; c < N_channel; ++c) {
					if(c < N_channel - 1)
						cout << r2cFFT.rawOutput[c + N_channel*f] << "\t";
					else
						cout << r2cFFT.rawOutput[c + N_channel*f] << endl;
				}
			}
			indexData = 0;
		}
	}

	return 0;
}
