#include <fcntl.h> // O_RDONLY O_WRONLY
#include "../include/simplePipe.h" // SimplePipe()
#include <string> // std::stod()
#include <unistd.h> // usleep(microseconds)
#include <iostream> // std::cerr std::endl;
using namespace std;
#ifndef SIZE
#define SIZE 8
#endif

int main(int argc, char* argv[]) {
	if(argc < 2) {
		cerr << "Usage: " << argv[0] << " <FIFO filename>";
		for(int i = 0; i < SIZE; ++i)
			cerr << " <double>";
		cerr << endl;
		return -1;
	}

	simplePipe<double, SIZE> test(argv[1], O_WRONLY);
	double message[SIZE];
	for(int i = 0; i < SIZE; ++i)
		message[i] = std::stod(argv[i+2]);

	while(true) {
		int writBytes = test.pipeOut((double (&)[SIZE])message);
		cout << "Bytes Written: " << writBytes << endl;
		usleep(1000);
	}
	return 0;
}
#undef SIZE
