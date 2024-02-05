#include <iostream> // std::cerr std::cout
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
	while(cin >> data[indexData]) {
		indexData++;
		if(indexData % N_channel == 0) {
			indexData = 0;
			fwrite(data, N_channel, sizeof(double), stdout);
		}
	}
	return 0;
}
