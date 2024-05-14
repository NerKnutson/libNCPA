#ifndef DETECTOR_H
#define DETECTOR_H

#include <mutex> // std::mutex
#include "buffers/inputBuffer.h"
#include "buffers/outputBuffer.h"

template <class S = double>
class detector {
	public:
		// Assume zero-th order for input
		detector(size_t numberSamples):
			input(numberSamples), output(numberSamples) { }
		detector(size_t numberSamplesInput, size_t numberSamplesOutput):
			input(numberSamplesInput), output(numberSamplesOutput) { }
		detector(size_t numberSamplesInput, size_t numberSamplesOutput, size_t howMany):
			input(numberSamplesInput,howMany), output(numberSamplesOutput,howMany) { }
		detector(size_t numberSamplesInput, size_t numberSamplesOutput, size_t howManyInput, size_t howManyOutput):
			input(numberSamplesInput,howManyInput), output(numberSamplesOutput,howManyOutput) { }

		// Expected to redefine hypothesis_test()
		virtual bool hypothesis_test() { return true; }

		template<class V>
		int read(V& fileInput) { return input.read(fileInput); }
		template<class V>
		int write(V& fileOutput) { return output.write(fileOutput); }

		inputBuffer<S> input;
		outputBuffer<bool> output;

	protected: // TODO: Determine if this needs to be protected
		double threshold = 0;
		std::mutex bufferLock;
};
#endif
