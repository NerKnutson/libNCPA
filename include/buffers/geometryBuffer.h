#ifndef GEOMETRY_BUFFER_H
#define GEOMETRY_BUFFER_H
// Include this class to have an input buffer
// #include "geometryBuffer.h"
// class foo {
//		public:
//			geometryBuffer publicBuffer;
//		private:
//			geometryBuffer privateBuffer;
// };

#include "BUFFER.h"
#include <iostream> // std::istream std::ostream
#include <mutex> // std::mutex

template <class S = double>
class geometryBuffer: public buffer<S> {
	public:
		geometryBuffer(size_t howMany, size_t dimensions): buffer<S>(dimensions, howMany) { }
/******************************************************************
 * read(): inputs geometry data into buffer
******************************************************************/
		inline int read(FILE* fileInput) {
			auto bufferContainer = this->getBuffer();
			bufferLock.lock();
			int bytesRead = fread(&bufferContainer.front(), sizeof(S), bufferContainer.size(), fileInput);
			bufferLock.unlock();
			return bytesRead;
		}
		inline int read(std::istream& fileInput) {
			auto bufferContainer = this->getBuffer();
			bufferLock.lock();
			for(auto &element : bufferContainer)
				fileInput >> element;
			bufferLock.unlock();
			return bufferContainer.size()*sizeof(S);
		}
		inline int read(std::span<S> input) {
			auto bufferContainer = this->getBuffer();
			if(input.size() < bufferContainer.size()) {
				bufferLock.lock();
				std::copy(input.begin(), input.end(), bufferContainer.begin());
				bufferLock.unlock();
				return input.size()*sizeof(S);
			}
			bufferLock.lock();
			std::copy(input.begin(), input.begin() + bufferContainer.size(), bufferContainer.begin());
			bufferLock.unlock();
			return bufferContainer.size()*sizeof(S);
		}
/**************************************************************
* The following gives nice container properties for the buffers
* But requires std=c++20
* - This is how the user is expected to interact with data
**************************************************************/
		auto getData() { return this->getBuffer(); }
		auto getData(int index) { return this->getBuffer(index); }

	private:
		std::mutex bufferLock;
};
#endif
