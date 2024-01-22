#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <vector>
#include "../include/rollingDFT.h"
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --lengthFT <length of Fourier Transform> [optional: --humanReadable] <bin indices>"  << endl;
		return 1;
	}
	unsigned N_channel = 0;
	unsigned lengthFT = 0;
	unsigned N_bin = argc - 5;
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--lengthFT") == 0 || strcmp(argv[arg], "-lFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0) {
			humanReadable = true;
			N_bin = argc - 6;
		}
	}
	if(N_channel <= 0 || lengthFT <= 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	vector<rollingDFT<double, complex<double>>> roller;
	roller.reserve(N_bin);
	for(int b = argc - N_bin; b < argc; ++b) {
		unsigned binIndex = stoi(argv[b]);
		if(binIndex < 0 || 2 * binIndex > lengthFT) {
			cerr << "Error in " << argv[0] << ":\nInvalid bin index" << endl;
			return 1;
		}
		roller.push_back(rollingDFT<double, complex<double>>(N_channel, lengthFT, binIndex));
	}

	double data[N_channel];
	fill(data, data + N_channel, 0.0);

	if(humanReadable) {
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		unsigned indexData = 0;
		while(cin >> data[indexData]) {
			indexData++;
			if(indexData%N_channel == 0) {
				indexData = 0;
				for(int b = 0; b < N_bin; ++b) {
					complex<double>* rOutput = roller[b].DFT((double*)data);
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
		while(fread(data, N_channel, sizeof(double), stdin)) {
			for(int b = 0; b < N_bin; ++b) {
				complex<double>* rOutput = roller[b].DFT((double*)&data);
				fwrite(rOutput, N_channel, sizeof(std::complex<double>), stdout);
			}
		}
	}

	roller.clear();

	return 0;
}
