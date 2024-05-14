#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H
// Include this class to have an input buffer
// #include "inputBuffer.h"
// class foo {
//		public:
//			inputBuffer publicBuffer;
//		private:
//			inputBuffer privateBuffer;
// };

#include "BUFFER.h"
#include <iostream> // std::istream std::ostream
#include <mutex> // std::mutex

template <class S = double>
class inputBuffer: public buffer<S> {
	public:
		inputBuffer(size_t numberSamples): buffer<S>(numberSamples) { }
		inputBuffer(size_t numberSamples, size_t howMany): buffer<S>(numberSamples,howMany) { }
/******************************************************************
 * read(): inputs new data into head
 * head is updated before read
******************************************************************/
		inline int read(FILE* fileInput) {
			++head;
			auto bufferContainer = this->getBuffer(head);
			bufferLock.lock();
			int bytesRead = fread(&bufferContainer.front(), sizeof(S), bufferContainer.size(), fileInput);
			bufferLock.unlock();
			return bytesRead;
		}
		inline int read(std::istream& fileInput) {
			++head;
			auto bufferContainer = this->getBuffer(head);
			bufferLock.lock();
			for(auto &element : bufferContainer)
				fileInput >> element;
			bufferLock.unlock();
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
