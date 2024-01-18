#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "../include/rollingDFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 6){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --lengthFT <length of Fourier Transform> [optional: --humanReadable] <bin indices>"  << endl;
		return 1;
	}
	unsigned N_channel = 0;
	unsigned lengthFT = 0;
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

	rollingDFT<double>* roller[N_bin];
	for(int b = 0; b < N_bin; ++b) {
		unsigned binIndex = stoi(argv[b + 5]);
		if(binIndex < 0 || 2 * binIndex > lengthFT) {
			cerr << "Error in " << argv[0] << ":\nInvalid bin index" << endl;
			return 1;
		}
		roller[b] = new rollingDFT<double>(N_channel, lengthFT, binIndex);
	}

	unsigned lengthInput = 2*lengthFT;
	double data[N_channel*lengthInput];
	fill(data, data + 2*N_channel*lengthFT, 0.0);
	unsigned indexData = 0;

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(cin >> data[indexData]) {
			indexData = (indexData + 1) % (2*N_channel*lengthFT);
			if(indexData%N_channel == 0) {
				for(int b = 0; b < N_bin; ++b) {
					complex<double>* rOutput = roller[b]->DFT((double*)&data, (indexData/N_channel - 1 + lengthInput) % lengthInput, (2*lengthFT));
					for(int c = 0; c < N_channel; ++c) {
						cout << rOutput[c];
						if(c < N_channel - 1)
							 cout << "\t";
						else
							cout << endl;
					}
				}
			}
		}
	} else {
		while(cin >> data[indexData]) {
		while(fread(r2cFFT.rawInput, sizeInput, sizeof(double), stdin)) {
			indexData = (indexData + 1) % (2*N_channel*lengthFT);
			if(indexData%N_channel == 0) {
				for(int b = 0; b < N_bin; ++b) {
					complex<double>* rOutput = roller[b]->DFT((double*)&data, (indexData/N_channel - 1 + lengthInput) % lengthInput, (2*lengthFT));
					for(int c = 0; c < N_channel; ++c) {
						cout << rOutput[c];
						if(c < N_channel - 1)
							 cout << "\t";
						else
							cout << endl;
					}
				}
			}
		}
	}

	for(int b = 0; b < N_bin; ++b)
		delete(roller[b]);

	return 0;
}
