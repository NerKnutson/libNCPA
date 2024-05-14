#ifndef DECIMATOR_H
#define DECIMATOR_H
// Decimates data stream:
//		N_channels*decimationFactor input
//			to N_channels output
//
// Typical Operation:
// decimator D(<params>);
// while(D.read(<FILE* or istream>)) {
//		bool check = D.transform();
//		if(check)
//			D.write(<FILE* or ostream>);
// }

#include "TRANSFORMER.h"
#include <Iir.h>

#ifndef ORDER
#define ORDER 5
#endif

template <class S = double, class T = double>
class decimator : public transformer<S,T> {
	public:
		decimator(size_t N_channels, unsigned decimationFactor, double sampleFrequency, double cutoffFrequency, double stopbandRipple) :
		transformer<S,T>(N_channels*decimationFactor, N_channels) {

			lowPass = new Iir::ChebyshevII::LowPass<ORDER>[N_channels];
			for(int c = 0; c < N_channels; ++c) {
				lowPass[c].setup(
				sampleFrequency, cutoffFrequency,
				stopbandRipple);
			}
		}

		~decimator() {
			delete[] lowPass;
		}

		bool transform() {
			auto head_I = this->input.getData();
			auto head_O = this->output.getData(); // Newest data
			// Filter points that are going to be decimated
			this->bufferLock.lock();
			for(size_t index = 0; index < head_I.size() - head_O.size(); ++index)
				lowPass[index%head_O.size()].filter(head_I[index]);
			// Save the undecimated point
			auto undecimated = head_I.end() - head_O.size();
			for(size_t index = 0; index < head_O.size(); ++index)
				head_O[index] = lowPass[index].filter(undecimated[index]);
			this->bufferLock.unlock();
			return true;
		}

	private:
		Iir::ChebyshevII::LowPass<ORDER>* lowPass;
};
#undef ORDER
#endif
