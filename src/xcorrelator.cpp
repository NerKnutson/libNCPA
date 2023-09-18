#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include "../include/xcorrelator.h" // xcorrelator(int,int)
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 9 && argc != 5){
		cerr << "Usage: " << argv[0] << " --N_channel <number of channels> --lengthFT <length of Fourier transform> [optional: --indexA <index of channel A> --indexB <index of channel B>]" << endl;
		return 1;
	}
	int N_channel = 0;
	int lengthFT = 0;
	int indexA = 0;
	int indexB = 0;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channel") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);
		else if(strcmp(argv[arg], "--lengthFT") == 0)
			sscanf(argv[arg + 1], "%d", &lengthFT);
		else if(strcmp(argv[arg], "--indexA") == 0)
			sscanf(argv[arg + 1], "%d", &indexA);
		else if(strcmp(argv[arg], "--indexB") == 0)
			sscanf(argv[arg + 1], "%d", &indexB);
	}

	if(N_channel < 0) {
		cerr << "Error in " << argv[0] << ":\nInvalid number of channels" << endl;
		return 1;
	}

	if(indexA < 0 || indexA > N_channel - 1 || indexB < 0 || indexB > N_channel - 1) {
		cerr << "Error in " << argv[0] << ":\nInvalid channel index" << endl;
		return 1;
	}

	int indexData = 0;
	xcorrelator XCORRELATOR(N_channel, lengthFT);
	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(cin >> XCORRELATOR.rawInput[indexData]) {
		indexData++;
		if(2 * indexData % (N_channel * lengthFT) == 0) {
			indexData = 0;
			XCORRELATOR.xcorrelate();
			int N_correlation = N_channel * (N_channel - 1) / 2;
			for(int s = 0; s < lengthFT; ++s) {
				if(indexA != indexB) { // assume that indices were passed to executable
					int pairNum = XCORRELATOR.pair_num(indexA, indexB);
					cout << XCORRELATOR.rawOutput[N_correlation * ((s + lengthFT/2) % lengthFT) + pairNum] << endl;
				} else {
					for(int c = 0; c < N_correlation; ++c) {
						if(c < N_correlation - 1)
							cout << XCORRELATOR.rawOutput[N_correlation * ((s + lengthFT/2) % lengthFT) + c] << "\t";
							//cout << XCORRELATOR.rawOutput[N_correlation*s + c] << "\t";
						else
							cout << XCORRELATOR.rawOutput[N_correlation * ((s + lengthFT/2) % lengthFT) + c] << endl;
							//cout << XCORRELATOR.rawOutput[N_correlation*s + c] << endl;
					}
				}
			}
		}
	}

	return 0;
}
