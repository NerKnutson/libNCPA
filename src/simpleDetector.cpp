#include "detectors/simpleDetector.h"
#include "transformers/timeStamper.h"

#include <cstring>
#include <chrono>
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5 || argc > 8){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --threshold <triggering mechanism (data > threshold)> [optional: --humanReadable] [optional: --timeStamp] [optional: --absoluteReference]" << endl;
		return 1;
	}

	unsigned N_channels = 1;
	double threshold = 1.0;
	bool humanReadable = false;
	bool timeStamp = false;
	bool absoluteReference = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--threshold") == 0 || strcmp(argv[arg], "-t") == 0)
			sscanf(argv[arg + 1], "%lf", &threshold);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
		else if(strcmp(argv[arg], "--timeStamp") == 0 || strcmp(argv[arg], "-tS") == 0)
			timeStamp = true;
		else if(strcmp(argv[arg], "--absoluteReference") == 0 || strcmp(argv[arg], "-aR") == 0)
			absoluteReference = true;
	}

	simpleDetector<double> detector(N_channels, threshold);
	timeStamper stamp(N_channels);
	if(absoluteReference)
		stamp.referenceTick = 0;
	if(timeStamp)
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
