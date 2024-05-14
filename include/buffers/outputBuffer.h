#ifndef OUTPUT_BUFFER_H
#define OUTPUT_BUFFER_H
// Include this class to have an output buffer
// class foo {
//		public:
//			outputBuffer publicBuffer;
//		private:
//			outputBuffer privateBuffer;
// };

#include "BUFFER.h"
#include <iostream> // std::istream std::ostream
#include <iomanip> // std::setprecision()
#include <limits> // std::numeric_limits<>
#include <mutex> // std::mutex

template <class S = double>
class outputBuffer: public buffer<S> {
	public:
		outputBuffer(size_t numberSamples): buffer<S>(numberSamples) {
			std::cout << std::setprecision(std::numeric_limits<S>::digits10 + 1);
		}
		outputBuffer(size_t numberSamples, size_t howMany): buffer<S>(numberSamples,howMany) {
			std::cout << std::setprecision(std::numeric_limits<S>::digits10 + 1);
		}
/******************************************************************
 * write(): outputs new data from head
 * head is updated after write
******************************************************************/
		inline int write(FILE* fileOutput) {
			auto bufferContainer = this->getBuffer(head);
			bufferLock.lock();
			int bytesWrit = fwrite(&bufferContainer.front(), sizeof(S), bufferContainer.size(), fileOutput);
			bufferLock.unlock();
			++head;
			return bytesWrit;
		}
		inline int write(std::ostream& fileOutput) {
			auto bufferContainer = this->getBuffer(head);
			bufferLock.lock();
			for(auto &element : bufferContainer) {
				fileOutput << element;
				if(&element != &bufferContainer.back())
					fileOutput << "\t";
				else
					fileOutput << "\n";
			}
			bufferLock.unlock();
			++head;
			return bufferContainer.size()*sizeof(S);
		}
/**************************************************************
* The following gives nice container properties for the buffers
* But requires std=c++20
* - This is how the user is expected to interact with data
**************************************************************/
		auto getData() { return this->getBuffer(head); }
		auto getData(int index) {
			index = head + index;
			return this->getBuffer(index);
		}

	private:
		// Location of most recent index
		int head = 0;
		std::mutex bufferLock;
};
#endif
