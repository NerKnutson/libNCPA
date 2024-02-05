#include <iostream> // std::cerr std::cout
#include <iomanip> // std::setprecision()
#include <limits> // numeric_limits<>
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 1 || argc > 2){
		cerr << "Usage: " << argv[0] << "[optional: <number of channels>]" << endl;
		return 1;
	}
	unsigned N_channel = 1;
	if(argc == 2)
		N_channel = atoi(argv[1]);

	double data[N_channel];
	fill(data, data + N_channel, 0.0);

	unsigned indexData = 0;
	cout << setprecision(numeric_limits<double>::digits10 + 1);
	while(fread(data, N_channel, sizeof(double), stdin)) {
		for(int c = 0; c < N_channel; ++c) {
			cout << data[c];
			if(c == N_channel - 1)
				cout << endl;
			else
				cout << "\t";
		}
	}

	return 0;
}
