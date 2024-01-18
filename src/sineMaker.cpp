#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <cmath> // std::sin()
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 5 && argc != 6){
		cerr << "Usage: " << argv[0] << " --frequency <frequency (Hz)> --sampleFrequency <sampleFrequency (Hz)> [optional: --humanReadable]" << endl;
		return 1;
	}
	double frequency = 0;
	double sampleFrequency = 0;
	bool humanReadable = false;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--frequency") == 0 || strcmp(argv[arg], "-f") == 0)
			sscanf(argv[arg + 1], "%lf", &frequency);
		else if(strcmp(argv[arg], "--sampleFrequency") == 0 || strcmp(argv[arg], "-sR") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleFrequency);
		else if(strcmp(argv[arg], "--humanReadable") == 0 || strcmp(argv[arg], "-hR") == 0)
			humanReadable = true;
	}

	if(2*frequency > sampleFrequency) {
		cerr << "Given frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	}

	if(humanReadable) { // Print human-readable data
		cout << setprecision(numeric_limits<double>::digits10 + 1);
		int s = 0;
		while(true) {
			double tmp = sin(2 * M_PI * frequency * s / sampleFrequency);
			cout << tmp << endl;
			s++;
		}
	} else { // Print binary representation
		int s = 0;
		while(true) {
			double tmp = sin(2 * M_PI * frequency * s / sampleFrequency);
			fwrite(&tmp, 1, sizeof(double), stdout);
			s++;
		}
	}

	return 0;
}
