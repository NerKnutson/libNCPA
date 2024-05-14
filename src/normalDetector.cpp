#include "detectors/normalDetector.h"
#include "transformers/timeStamper.h"

#include <cstring>
#include <chrono>
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 7 || argc > 9){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --learningRate <rate for learning mean and variance (0 < r < 1)> --pValue <p-value for hypothesis test (0 < p < 1)> [optional: --humanReadable] [optional: --timeStamp]" << endl;
		return 1;
	}

	unsigned N_channels = 1;
	double learningRate = 1.0;
	double pValue = 1.0;
	bool humanReadable = false;
	bool timeStamp = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--learningRate") == 0 || strcmp(argv[arg], "-lR") == 0)
			sscanf(argv[arg + 1], "%lf", &learningRate);
		else if(strcmp(argv[arg], "--pValue") == 0 || strcmp(argv[arg], "-pV") == 0)
			sscanf(argv[arg + 1], "%lf", &pValue);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
		else if(strcmp(argv[arg], "--timeStamp") == 0 || strcmp(argv[arg], "-tS") == 0)
			timeStamp = true;
	}

	normalDetector<double> detector(N_channels, learningRate, pValue);
	timeStamper stamp(N_channels);
	if(timeStamp) // Share buffer if timeStamp flag
		stamp.input.steal(detector.output);

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(detector.read(std::cin)) {
			detector.hypothesis_test();
			timeStamp && stamp.transform();
			timeStamp
				&& stamp.write(std::cout)
				|| detector.write(std::cout);
		}
	} else { // Print binary representation
		while(detector.read(stdin)) {
			detector.hypothesis_test();
			timeStamp && stamp.transform();
			timeStamp
				&& stamp.write(stdout)
				|| detector.write(stdout);
		}
	}
}
