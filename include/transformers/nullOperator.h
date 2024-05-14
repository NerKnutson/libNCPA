#ifndef NULL_OPERATOR_H
#define NULL_OPERATOR_H
// Takes input and saves it to buffer
// N_samples input to N_samples output (output is the same buffer)
//
// Typical Operation:
// nullBuffer<<type>> X(<params>);
// while(X.read(<FILE* or istream>)) {
//		X.transform(); // this is not a necessary function call
//		X.write(<FILE* or ostream>);
// }

#include "TRANSFORMER.h"

template <class S = double>
class nullOperator : public transformer<S,S> {
	public:
		nullOperator(size_t N_samples):
			transformer<S,S>(N_samples) { this->inPlace(); }
		bool transform() { return this->inPlace(); }
};

#endif
