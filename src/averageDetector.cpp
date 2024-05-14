#include "detectors/averageDetector.h"
#include "transformers/timeStamper.h"

#include <cstring>
#include <chrono>
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 7 || argc > 12){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --shortLength <length of short term average> --longLength <length of long term average> [optional: --ratio <ratio of short term average to long term average to trigger>] [optional: --humanReadable] [optional: --timeStamp] [optional: --absoluteReference]" << endl;
		return 1;
	}

	unsigned N_channels = 1;
	unsigned shortLength = 0;
	unsigned longLength = 0;
	double ratio = 1.0;
	bool humanReadable = false;
	bool timeStamp = false;
	bool absoluteReference = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--shortLength") == 0 || strcmp(argv[arg], "-sL") == 0)
			sscanf(argv[arg + 1], "%d", &shortLength);
		else if(strcmp(argv[arg], "--longLength") == 0 || strcmp(argv[arg], "-lL") == 0)
			sscanf(argv[arg + 1], "%d", &longLength);
		else if(strcmp(argv[arg], "--ratio") == 0 || strcmp(argv[arg], "-r") == 0)
			sscanf(argv[arg + 1], "%lf", &ratio);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
		else if(strcmp(argv[arg], "--timeStamp") == 0 || strcmp(argv[arg], "-tS") == 0)
			timeStamp = true;
		else if(strcmp(argv[arg], "--absoluteReference") == 0 || strcmp(argv[arg], "-aR") == 0)
			absoluteReference = true;
	}
	if(shortLength == 0 || longLength == 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid buffer size(s)" << endl;
		return 1;
	}
	averageDetector<double> detector(N_channels, shortLength, longLength, ratio);
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
