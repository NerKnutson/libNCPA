#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <random> // std::default_random_engine generator std::normal_distribution<double> dist(double, double)
#include <Iir.h> // Iir::Butterworth::LowPass<int>
#include <unistd.h> // std::usleep(int);
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 5 && argc != 7){
		cerr << "Usage: " << argv[0] << " --centerFrequency <frequency (Hz)> --sampleRate <sampleRate (Hz)> [optional: --widthFrequency <frequency Hz>]" << endl;
		return 1;
	}
	double centerFrequency = -1; // Hz
	double sampleRate = -1; // Hz
	double widthFrequency = 10; // Hz

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--centerFrequency") == 0)
			sscanf(argv[arg + 1], "%lf", &centerFrequency);
		else if(strcmp(argv[arg], "--sampleRate") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleRate);
		else if(strcmp(argv[arg], "--widthFrequency") == 0)
			sscanf(argv[arg + 1], "%lf", &widthFrequency);
	}
	if(2*centerFrequency > sampleRate) {
		cerr << "Given frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	} else if(centerFrequency < 0) {
		cerr << "Invalid frequency: must be > 0" << endl;
		return 1;
	} else if(sampleRate < 0 ) {
		cerr << "Invalid sampleRate: must be > 0" << endl;
		return 1;
	}

	Iir::Butterworth::BandPass<3> bandPass;
	bandPass.setup(sampleRate, centerFrequency, widthFrequency);

	default_random_engine generator;
	generator.seed(time(NULL));
	normal_distribution<double> dist(0.0, 1.0);

	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(true)
		cout << bandPass.filter(dist(generator)) << endl;

	return 0;
}
