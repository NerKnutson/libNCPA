#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp()
#include <vector> // std::vector<>
#include <complex> // std::complex<>
#include "transformers/nullOperator.h"
using namespace std;
template<class T>
void routine(nullOperator<T>&);

int main(int argc, char* argv[]) {
	if(argc < 1 || argc > 5){
		cerr << "Usage: " << argv[0] << "[optional: --N_channels <number of channels>] [optional: --type <numeric type>]" << endl;
		return 1;
	}

	unsigned N_channels = 1;
	for(int arg = 0; arg < argc; ++arg)
		if(strcmp(argv[arg], "--N_channels") == 0 || strcmp(argv[arg], "-Nc") == 0)
			sscanf(argv[arg + 1], "%d", &N_channels);

	// Scans command-line arguments for type
	for(int arg = 0; arg < argc; ++arg) {
		if(strcmp(argv[arg], "--type") == 0 || strcmp(argv[arg], "-t") == 0) {
			if(strcmp(argv[arg + 1], "double") == 0) {
				nullOperator<double> data(N_channels);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "std::complex<double>") == 0 || strcmp(argv[arg + 1], "complex double") == 0 || strcmp(argv[arg + 1], "complex") == 0) {
				nullOperator<complex<double>> data(N_channels);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "integer") == 0 || strcmp(argv[arg + 1], "int") == 0) {
				nullOperator<int> data(N_channels);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "long") == 0) {
				nullOperator<long> data(N_channels);
				routine(data);
				return 0;
			} else if(strcmp(argv[arg + 1], "boolean") == 0 || strcmp(argv[arg + 1], "bool") == 0) {
				nullOperator<bool> data(N_channels);
				routine(data);
				return 0;
			}
		}
	}

	// If no type; then assume double
	nullOperator<double> data(N_channels);
	routine(data);
	return 0;
}

// Routines
template<class T>
void routine(nullOperator<T> &data) {
	while(data.read(std::cin))
		if(data.write(stdout) < 0)
			break;
}
