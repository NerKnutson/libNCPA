#include "transformers/timeStamper.h"

#include <cstring>
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 3 || argc > 5){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> [optional: --humanReadable] [optional: --absoluteReference]" << endl;
		return 1;
	}

	unsigned N_channels = 1;
	bool humanReadable = false;
	bool absoluteReference = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
		else if(strcmp(argv[arg], "--absoluteReference") == 0 || strcmp(argv[arg], "-aR") == 0)
			absoluteReference = true;
	}

	timeStamper stamp(N_channels);
	if(absoluteReference)
		stamp.referenceTick = 0;

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(stamp.read(std::cin)) {
			if(!stamp.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(stamp.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(stamp.read(stdin)) {
			if(!stamp.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(stamp.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}
}
