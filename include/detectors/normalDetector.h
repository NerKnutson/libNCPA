#ifndef NORMAL_DETECTOR_H
#define NORMAL_DETECTOR_H

#include <cmath> // std::sqrt() std::log()
#include <vector> // std::vector<>
#include "DETECTOR.h"
#ifdef DEBUG
#include <iostream> // std::cerr
#endif

template <class S = double>
class normalDetector : public detector<S> {
	public:
		normalDetector(unsigned numberChannels, double learnRate, double pValue) :
		detector<S>(numberChannels),
		moments(numberChannels, std::vector<double>(2)),
		learnRate(learnRate) {
			this->threshold = inverseCDF(pValue);
			this->threshold *= this->threshold; // Stores square of value as threshold
			for(auto& channels : moments) { // Sets initial statistics for standard normal N(0,1)
				channels[0] = 0; // Set first moment to 0 initially
				channels[1] = 1; // Set second moment to 1 initially
			}
		}

		bool hypothesis_test() {
			bool anyTriggered = false;
			auto head_I = this->input.getData();
			auto head_O = this->output.getData();
			this->bufferLock.lock();
			for(int c = 0; c < head_O.size(); ++c) {
				// Normalize incoming data
				double tmp = head_I[c] - moments[c][0];
				double var = moments[c][1] - moments[c][0] * moments[c][0];
				if(tmp*tmp > var * this->threshold) {
					head_O[c] = true;
					anyTriggered = true;
				} else {
					head_O[c] = false;
					tmp = head_I[c];
				}
				for(auto& moment: moments[c]) {
					moment = (1.0 - learnRate) * moment + learnRate * tmp;
					tmp *= head_I[c];
				}
			}
			this->bufferLock.unlock();
			return anyTriggered;
		}
	private:
		std::vector<std::vector<S>> moments;
		double learnRate;

		double inverseCDF(double pValue) {
			// Handbook of Mathematical Functions
			// Abramowitz and Stegun
			// Equation 26.2.23
			// |error(p)| < 4.5e-4
			double t = std::sqrt(-2.0 * std::log(pValue));
			double num = 2.515517 + 0.802853*t + 0.010328*t*t;
			double den = 1.0 + 1.432788*t + 0.189269*t*t + 0.001308*t*t*t;
#ifdef DEBUG
			std::cerr << std::setprecision(std::numeric_limits<S>::digits10 + 1);
			std::cerr << "[normalDetector] (pValue, zValue): "
				<< "(" << pValue << ", " << t - num/den  << ")" << std::endl;
#endif
			return t - num/den;
		}
};
#endif
