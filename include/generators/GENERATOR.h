#ifndef GENERATOR_H
#define GENERATOR_H

#include "buffers/outputBuffer.h"
#include <mutex>

template <class S = double>
class generator {
	public:
		// Assume zero-th order for input
		generator(size_t numberSamples): output(numberSamples) { }
		generator(size_t numberSamples, size_t howMany): output(numberSamples,howMany) { }

		// Expected to redefine hypothesis_test()
		virtual int generate() { return true; }

		template<class V>
		int write(V& fileOutput) { return output.write(fileOutput); }

	//protected: // TODO: Determine if this needs to be protected
		outputBuffer<S> output;

	protected:
		std::mutex bufferLock;
};
#endif
