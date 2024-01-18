// TODO: Fix this to not need these defines
#define N_channel 128
#define N_subchannel 7
#define N_frame 16
#define N_bin 10
#define lengthFT 512

#include <armadillo>
#include <vector>
#include <atomic>
#include "simplePipe.h"

std::atomic<unsigned> timeStamp(0);
void timeThread() {
	simplePipe<double,4> xdaqGPS("/tmp/xdaqGPS", O_RDONLY);
	double msgIn[4];
	while(true) {
		int bytes = xdaqGPS.pipeIn(msgIn);
		if(bytes > 0) {
			timeStamp = msgIn[0];
			cout << timeStamp << endl;
		}
	}
}

std::atomic<bool> qReady(false);
std::mutex dataMutex;
std::condition_variable dataReady;

void daqThread(arma::cx_cube &main, std::vector<uword> &micList) {
	arma::cx_cube dataIn(N_channel, N_frame, N_bin);

	unsigned indexChannel = 0;
	unsigned indexBin = 0;
	unsigned indexFourier = 0;
	unsigned indexFrame = 0;
	while(cin >> dataIn(indexChannel, indexFrame, indexBin)) {
		indexChannel++;
		if(indexChannel == N_channel) {
			indexChannel = 0;
			indexBin++;
		}
		if(indexBin == N_bin) {
			indexBin = 0;
			indexFourier++;
		}
		// only save every <lengthFT> because of use of sliding FT
		if(indexFourier == lengthFT) {
			indexFourier = 0;
			indexFrame++;
		}
		if(indexFrame == N_frame) {
			indexFrame = 0;
			if(qReady == true) {
				// copy to garth
				std::unique_lock<std::mutex> lk(dataMutex);
				for(int c = 0; c < N_subchannel; ++c)
					for(int b = 0; b < N_bin; ++b)
						for(int f = 0; f < N_frame; ++f)
							main(c,f,b) = dataIn(micList(c),f,b);
				qReady = false;
				lk.unlock();
				dataReady.notify_one();
			}
		}
	}
}
/*
std::thread DAQ(daqThread, cube, list);
std::thread TIME(timeThread);

DAQ.join();
TIME.join();
*/
