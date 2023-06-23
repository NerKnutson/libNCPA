#ifndef DECIMATOR_H
#define DECIMATOR_H

#include <thread>
#include <mutex>

#include <Iir.h>

#ifndef ORDER
#define ORDER 5
#endif

class decimator {
	public:
		decimator(int N_channel, int decimationFactor, double sampleRate, double cutoffFrequency, double stopbandRipple):
			N_channel(N_channel),
			decimationFactor(decimationFactor),
			decimationState(0) {
				rawOutput = new double [N_channel];
				low_pass = new Iir::ChebyshevII::LowPass<ORDER>[N_channel];
				for(int c = 0; c < N_channel; ++c) {
					low_pass[c].setup(
					sampleRate, cutoffFrequency,
					stopbandRipple);
				}
			}

		~decimator() {
			delete[] rawOutput;
			delete[] low_pass;
		}

		double* decimate(double input[]) {
			++decimationState;
			if(decimationState == decimationFactor) {
				for(int c = 0; c < N_channel; ++c) {
					rawOutput[c] = low_pass[c].filter(input[c]);
					decimationState = 0;
				}
				return rawOutput;
			}
			for(int c = 0; c < N_channel; ++c)
				low_pass[c].filter(input[c]);
			return NULL;
		}
		unsigned decimationState;
	private:
		double* rawInput;
		double* rawOutput;

		Iir::ChebyshevII::LowPass<ORDER>* low_pass;

		const int N_channel;
		const int decimationFactor;

		std::mutex bufferLock;
		bool Q_handle_output;
};

#undef ORDER
#endif
