#ifndef NORM_CALCULATOR_H
#define NORM_CALCULATOR_H
// Takes input of std::complex<S> and returns std::norm of those values
// N_samples input to N_samples output
//
// Typical Operation:
// normCalculator<<type>> X(<params>);
// while(X.read(<FILE* or istream>)) {
//		X.transform(); // this is not a necessary function call
//		X.write(<FILE* or ostream>);
// }

#include <complex>
#include "TRANSFORMER.h"
template <class S = double>
class normCalculator : public transformer<std::complex<S>, S> {
	public:
		normCalculator(size_t numberChannels):
		transformer<std::complex<S>,S>(numberChannels) { }
		bool transform() {
			auto head_I = this->input.getData();
			auto head_O = this->output.getData();
			this->bufferLock.lock();
			for(size_t index = 0; index < head_I.size(); ++index) { head_O[index] = std::norm(head_I[index]); }
			this->bufferLock.unlock();
			return true;
		}
	private:
};

#endif
