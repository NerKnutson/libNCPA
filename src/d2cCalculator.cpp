#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/distanceToCoordinate.h" // distanceToCoordinate(int,int)
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 5 && argc != 6){
		cerr << "Usage: " << argv[0] << " --N_channels <number of channels> --N_dimensions <number of dimensions> [optional: --humanReadable]"  << endl;
		return 1;
	}
	int N_channels = 0;
	int N_dimensions = 0;
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);
		else if(strcmp(argv[arg], "--N_dimensions") == 0 || strcmp(argv[arg], "-Nd") == 0)
			sscanf(argv[arg + 1], "%d", &N_dimensions);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
	}

	if(N_channels < 1) {
		cerr << "Error in " << argv[0] << ":\nInvalid number of channels" << endl;
		return 1;
	}
	if(N_dimensions < 1) {
		cerr << "Error in " << argv[0] << ":\nInvalid number of channels" << endl;
		return 1;
	}

	distanceToCoordinate<double> D2C(N_channels, N_dimensions);

	if(humanReadable) { // Print human-readable data
		while(D2C.read(std::cin)){
			if(!D2C.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(D2C.write(std::cout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	} else { // Print binary representation
		while(D2C.read(stdin)){
			if(!D2C.transform()) {
				cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
				return 1;
			}
			if(D2C.write(stdout) < 0) {
				cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
				return 1;
			}
		}
	}

	return 0;
}
