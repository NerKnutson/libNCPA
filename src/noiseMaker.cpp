#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <random> // std::default_random_engine generator std::normal_distribution<double> dist(double, double)
#include <Iir.h> // Iir::Butterworth::LowPass<int>
#include <unistd.h> // std::usleep(int);
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 5 || argc > 8){
		cerr << "Usage: " << argv[0] << " --centerFrequency <frequency (Hz)> --sampleFrequency <sampleFrequency (Hz)> [optional: --widthFrequency <frequency Hz>] [optional: --humanReadable]" << endl;
		return 1;
	}
	double centerFrequency = -1; // Hz
	double sampleFrequency = -1; // Hz
	double widthFrequency = 10; // Hz
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--centerFrequency") == 0 || strcmp(argv[arg], "-cF") == 0)
			sscanf(argv[arg + 1], "%lf", &centerFrequency);
		else if(strcmp(argv[arg], "--sampleFrequency") == 0 || strcmp(argv[arg], "-sF") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleFrequency);
		else if(strcmp(argv[arg], "--widthFrequency") == 0 || strcmp(argv[arg], "-wF") == 0)
			sscanf(argv[arg + 1], "%lf", &widthFrequency);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
	}
	if(2*centerFrequency > sampleFrequency) {
		cerr << "Given frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	} else if(centerFrequency < 0) {
		cerr << "Invalid frequency: must be > 0" << endl;
		return 1;
	} else if(sampleFrequency < 0 ) {
		cerr << "Invalid sampleFrequency: must be > 0" << endl;
		return 1;
	}

	Iir::Butterworth::BandPass<3> bandPass;
	bandPass.setup(sampleFrequency, centerFrequency, widthFrequency);

	default_random_engine generator;
	generator.seed(time(NULL));
	normal_distribution<double> dist(0.0, 1.0);

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		while(true)
			cout << bandPass.filter(dist(generator)) << endl;
	} else { // Print binary representation
		while(true) {
			double tmp = bandPass.filter(dist(generator));
			fwrite(&tmp, 1, sizeof(double), stdout);
		}
	}

	return 0;
}
