#ifndef SIMPLE_DETECTOR_H
#define SIMPLE_DETECTOR_H

#include "DETECTOR.h"

template <class S = double>
class simpleDetector : public detector<S> {
	public:
		simpleDetector(unsigned numberChannels, double maxThreshold)
		: detector<S>(numberChannels) {
			this->threshold = maxThreshold*maxThreshold; // Store square of value as threshold
		}

		bool hypothesis_test() {
			bool anyTriggered = false;
			auto head_I = this->input.getData();
			auto head_O = this->output.getData();
			this->bufferLock.lock();
			for(int c = 0; c < head_O.size(); ++c) {
				if(head_I[c]*head_I[c] > this->threshold) {
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
};
#endif
