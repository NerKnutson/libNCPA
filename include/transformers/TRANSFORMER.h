#ifndef TRANSFORMER_H
#define TRANSFORMER_H
// Inherited this class to be a "transformer"
// class foo : public transformer (<params>)
// Typical Operation:
// foo X(<params>);
// while(X.read(<FILE* or istream>)) {
//		bool check = X.transform();
//		if(check)
//			X.write(<FILE* or ostream>);
// }
//

#include <iostream> // std::istream std::ostream
#include <iomanip> // std::setprecision()
#include <limits> // std::numeric_limits<>
#include <span> // std::span()
#include <mutex> // std::mutex

#include "buffers/inputBuffer.h"
#include "buffers/outputBuffer.h"

template <class S = double, class T = double>
class transformer {
	public:
		transformer(size_t numberSamples):
			input(numberSamples), output(numberSamples) { }
		transformer(size_t numberSamplesInput, size_t numberSamplesOutput):
			input(numberSamplesInput), output(numberSamplesOutput) { }
		transformer(size_t numberSamplesInput, size_t numberSamplesOutput, size_t howMany):
			input(numberSamplesInput,howMany), output(numberSamplesOutput,howMany) { }
		transformer(size_t numberSamplesInput, size_t numberSamplesOutput, size_t howManyInput, size_t howManyOutput):
			input(numberSamplesInput,howManyInput), output(numberSamplesOutput,howManyOutput) { }

		// Expected to redefine transform()
		virtual bool transform() {
			/*
			auto inputData = input.getData(); // Recently read data
			auto outputData = output.getData(); // Recently writ data
			auto nextData = output.getData(1); // Get next data
			std::copy(&inputData.front(), &inputData.back()+1, &nextData.front());
			*/
			return true;
		}

		template<class V>
		int read(V& fileInput) { return input.read(fileInput); }
		template<class V>
		int write(V& fileOutput) { return output.write(fileOutput); }

		bool inPlace() { return input.steal(output); }

		inputBuffer<S> input;
		outputBuffer<T> output;

	protected:
		std::mutex bufferLock;
};
#endif
