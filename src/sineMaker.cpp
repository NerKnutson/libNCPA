#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <cmath> // std::sin()
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 5){
		cerr << "Usage: " << argv[0] << " --frequency <frequency (Hz)> --sampleRate <sampleRate (Hz)>" << endl;
		return 1;
	}
	double frequency = 0;
	double sampleRate = 0;

	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--frequency") == 0)
			sscanf(argv[arg + 1], "%lf", &frequency);
		else if(strcmp(argv[arg], "--sampleRate") == 0)
			sscanf(argv[arg + 1], "%lf", &sampleRate);
	}

	if(2*frequency > sampleRate) {
		cerr << "Given frequency and sample rate do not satisfy Nyquist-Shannon theorem" << endl;
		return 1;
	}

	cout << setprecision(numeric_limits<double>::digits10 + 1);
	int s = 0;
	while(true) {
		double arg = frequency * s / sampleRate;
		cout << sin(2 * M_PI * arg) << endl;
		if(trunc(arg) == arg) // reset if arg is an integer
			s = 0;
		s++;
	}

	return 0;
}
