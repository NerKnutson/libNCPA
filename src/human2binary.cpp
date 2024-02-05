#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp()
#include <vector> // std::vector<>
#include <complex> // std::complex<>
using namespace std;
void routine(vector<double>);
void routine(vector<complex<double>>);
void routine(vector<int>);

int main(int argc, char* argv[]) {
	if(argc < 1 || argc > 5){
		cerr << "Usage: " << argv[0] << "[optional: --N_channel <number of channels>] [optional: --type <numeric type>]" << endl;
		return 1;
	}

	unsigned N_channel = 1;
	for(int arg = 0; arg < argc; ++arg)
		if(strcmp(argv[arg], "--N_channel") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channel);

	// Scans command-line arguments for type
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--type") == 0 || strcmp(argv[arg], "-t") == 0) {
			if(strcmp(argv[arg + 1], "double") == 0) {
				vector<double> data(N_channel,0);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "std::complex<double>") == 0 || strcmp(argv[arg + 1], "complex double") == 0 || strcmp(argv[arg + 1], "complex") == 0) {
				vector<complex<double>> data(N_channel,0);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "integer") == 0 || strcmp(argv[arg + 1], "int") == 0) {
				vector<int> data(N_channel,0);
				routine(data);
				return 0;
			}
		}
	}

	// If no type; then assume double
	vector<double> data(N_channel,0);
	routine(data);
	return 0;
}

// Routines
void routine(vector<double> data) {
	unsigned N_channel = data.size();
	unsigned indexData = 0;
	while(cin >> data[indexData]) {
		indexData++;
		if(indexData % N_channel == 0) {
			indexData = 0;
			fwrite(&data[0], N_channel, sizeof(double), stdout);
		}
	}
}

void routine(vector<complex<double>> data) {
	unsigned N_channel = data.size();
	unsigned indexData = 0;
	while(cin >> data[indexData]) {
		indexData++;
		if(indexData % N_channel == 0) {
			indexData = 0;
			fwrite(&data[0], N_channel, sizeof(complex<double>), stdout);
		}
	}
}

void routine(vector<int> data) {
	unsigned N_channel = data.size();
	unsigned indexData = 0;
	while(cin >> data[indexData]) {
		indexData++;
		if(indexData % N_channel == 0) {
			indexData = 0;
			fwrite(&data[0], N_channel, sizeof(int), stdout);
		}
	}
}
