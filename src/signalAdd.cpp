#define MAX_CHANNELS 128

#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setw(int)
#include <random> // std::default_random_engine generator std::normal_distribution<double> dist(double, double)
#include <climits> // INT_MAX
#include <cstring> // std::strcmp(char*, char*)
#include "../include/importArrayGeometry.h"
using namespace std;


int main(int argc, char* argv[]) {
	if(argc != 5 && argc !=7){
		cerr << "Usage: " << argv[0] << " --signalA <signal A filename.tsv> --signalB <signal B filename.tsv> [optional: --N_channel <number of channels>]\n" << endl;
		cerr << "Note: this program combines data from signal A and signal B with no regard for channel indexing.\n" << endl;
		return 1;
	}

	FILE* signalA = NULL;
	FILE* signalB = NULL;
	int N_channel = 1;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--signalA") == 0)
			signalA = fopen(argv[arg + 1], "r");
		else if(strcmp(argv[arg], "--signalB") == 0)
			signalB = fopen(argv[arg + 1], "r");
		else if(strcmp(argv[arg], "--N_channel") == 0)
			N_channel = stoi(argv[arg + 1]);
	}
	if(signalA == NULL && signalB == NULL) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	} else if(signalA == NULL) {
		signalA = signalB;
	} else if(signalB == NULL) {
		signalB = signalA;
	}

	double sampleA;
	double sampleB;

	int index = 0;
	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(!feof(signalA) || !feof(signalB)) {
		for(int c = 0; c < N_channel; ++c) {
			if(c < N_channel - 1) {
				if(!fscanf(signalA, "%lf\t", &sampleA))
					sampleA = 0.0;
				if(!fscanf(signalB, "%lf\t", &sampleB))
					sampleB = 0.0;
				cout << sampleA + sampleB << "\t";
			} else {
				if(!fscanf(signalA, "%lf\n", &sampleA))
					sampleA = 0.0;
				if(!fscanf(signalB, "%lf\n", &sampleB)) {
					sampleB = 0.0;
					cout << index++ << endl;
				}
				cout << sampleA + sampleB << endl;
			}
		}
	}

	return 0;
}
