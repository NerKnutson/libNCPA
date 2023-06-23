#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <cstring> // std::strcmp(char*,char*)
#include <random> // std::default_random_engine generator std::normal_distribution<double> dist(double, double)
using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 3 && argc != 1){
		cerr << "Usage: " << argv[0] << " --N_sample <number of samples>"  << endl;
		return 1;
	}

	int N_sample = 0;
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--N_sample") == 0)
			sscanf(argv[arg + 1], "%d", &N_sample);
	}
	if(N_sample < 0) {
		cerr << "Error in " << argv[0] << ":\n Invalid command-line arguments" << endl;
		return 1;
	}

	const double mean = 0.0;
	const double stddev = 1.0;
	default_random_engine generator;
	normal_distribution<double> dist(mean, stddev);

	int channel = 0;

	cout << setprecision(numeric_limits<double>::digits10 + 1);
	if(N_sample == 0)
		while(true)
			cout << dist(generator) << endl;
	else
		for(int s = 0; s < N_sample; ++s)
			cout << dist(generator) << endl;

	return 0;
}
