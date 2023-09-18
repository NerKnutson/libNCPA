#include <fcntl.h> // O_RDONLY O_WRONLY
#include "../include/simplePipe.h" // SimplePipe()
#include <string> // std::stod()
#include <iostream>
#include <iomanip>
using namespace std;
#ifndef SIZE
#define SIZE 1
#endif

int main(int argc, char* argv[]) {
	if(argc < 2) {
		cerr << "Usage: " << argv[0] << " <FIFO filename>";
		cerr << endl;
		return -1;
	}

	simplePipe<double, SIZE> test(argv[1], O_RDONLY);
	double message[SIZE];
	while(true) {
		int bytes = test.pipeIn((double (&)[SIZE])message);
		if(bytes > 0) {
			cout << "Message: ";
			for(int i = 0; i < SIZE; ++i)
				cout << setw(16) << message[i];
			cout << endl << "Bytes read: " << bytes << endl;
		}
		else{
			usleep(1000);
		}
	}
	return 0;
}

#undef SIZE
