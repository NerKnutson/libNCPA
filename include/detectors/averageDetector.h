#ifndef AVERAGE_DETECTOR_H
#define AVERAGE_DETECTOR_H

#include "DETECTOR.h"

template <class S = double>
class averageDetector : public detector<S> {
	public:
		averageDetector(unsigned numberChannels, unsigned shortLength, unsigned longLength)
		: detector<S>(numberChannels, numberChannels, longLength + 1, 1),
		N_short(shortLength), N_long(longLength) {
			shortAverage = new double[numberChannels];
			longAverage = new double[numberChannels];
			this->threshold = 1; // Default ratio for triggering STA/LTA > ratio
		}
		averageDetector(unsigned numberChannels, unsigned shortLength, unsigned longLength, double ratio)
		: detector<S>(numberChannels, numberChannels, longLength + 1, 1),
		N_short(shortLength), N_long(longLength) {
			shortAverage = new double[numberChannels];
			longAverage = new double[numberChannels];
			this->threshold = ratio; // Ratio for triggering STA/LTA > ratio
		}
		~averageDetector() {
			delete[] shortAverage;
			delete[] longAverage;
		}

		bool hypothesis_test() {
			bool anyTriggered = false;
			auto head_I = this->input.getData(); // most recent data point
			auto shortTail = this->input.getData(-N_short); // tail end of short term buffer
			auto longTail = this->input.getData(-N_long); // tail end of long term buffer
			auto head_O = this->output.getData();
			this->bufferLock.lock();
			for(int c = 0; c < head_O.size(); ++c) {
				shortAverage[c] += (head_I[c] - shortTail[c])/N_short;
				longAverage[c] += (head_I[c] - longTail[c])/N_long;
				if(shortAverage[c] > longAverage[c] * this->threshold) {
					head_O[c] = true;
					anyTriggered = true;
				} else {
					head_O[c] = false;
				}
			}
			this->bufferLock.unlock();
			return anyTriggered;
		}
	private:
		double * shortAverage;
		double * longAverage;
		const int N_short;
		const int N_long;
};
#endif
