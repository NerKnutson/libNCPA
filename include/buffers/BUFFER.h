#ifndef BUFFER_H
#define BUFFER_H
// Inherited this class to be a "buffer"
// class fooBuffer : public buffer (<params>)

#include <span> // std::span()
#include <mutex> // std::mutex
#ifdef DEBUG
#include <iostream>
#endif

/*******************************************************
// Buffer appears like the following:
// |<                   sizeRaw                       >|
// |<N_samples> <N_samples> <N_samples> ... <N_samples>|
// \__________________________________________________/
//                   <howMany>
*******************************************************/

template <class S = double>
class buffer {
	public:
		buffer(size_t numberSamples):
		N_samples(numberSamples), N_chunks(1) {
			raw = new S[N_samples];
			std::fill(raw, raw + N_samples, 0);
		}
		buffer(size_t numberSamples, size_t howMany):
		N_samples(numberSamples), N_chunks(howMany) {
			raw = new S[N_samples*N_chunks];
			std::fill(raw, raw + N_samples*N_chunks, 0);
		}
		~buffer() { if(ownMemory) delete[] raw; }
/**************************************************************
* The following gives nice container properties for the buffers
* But requires std=c++20
* - This is how the user is expected to interact with data
* - This will correct your desired index to the actual index
*   - Note: This is why it is a pass-by-reference function
**************************************************************/
		inline auto getBuffer(int& index) {
			index %= N_chunks;
			if(index < 0)
				index += N_chunks;
			return std::span(raw + index*N_samples, N_samples - N_zeroes);
		}
		inline auto getBuffer() { // gives whole array
			return std::span(raw, N_samples*N_chunks);
		}
/*****************************************************
* Adjust howMany chunks of N_samples are in the buffer
*****************************************************/
		bool adjustSamples(size_t numberSamples, size_t howMany) {
			this->N_samples = numberSamples;
			this->N_chunks = howMany;
			return resizeBuffer(N_chunks*N_samples);
		}
/*******************************
* Adjust zero padding for buffer
*******************************/
		bool zeroPad(int numberZeroes) {
			if(numberZeroes >= N_samples || numberZeroes < 0)
				return false;
			N_zeroes = numberZeroes;
			return true;
		}
/***************************************************************************
* Handles underlying memory between <this> and <that> buffers:
* - Reclaim:
*   - maintains ownership of memory
*   - allocates new memory if needed.
* - Steal:
*   - reclaims memory
*   - Forces <that> memory to point at <this> memory
* - Borrow:
*   - loses ownership of memory
*   - Forces <this> memory to point at <that> memory.
*     - 'borrows' <that> memory
***************************************************************************/
		bool reclaim() {
			if(ownMemory)
				return true;
			ownMemory = true;
			bufferLock.lock();
			resizeBuffer(N_chunks*N_samples);
			bufferLock.unlock();
			return true;
		}
		bool steal(buffer& that) {
			if(!ownMemory) // Make sure that I own the memory
				this->reclaim();

			// Force them to borrow my memory
			return that.borrow(*this);
		}
		bool borrow(buffer& that) {
			auto mine = this->getBuffer();
			auto theirs = that.getBuffer();
			if(mine.size() != theirs.size()) // fail if incompatible sizes
				return false;

			if(!ownMemory) { // if no memory owned, then simply use their buffer
				raw = &theirs.front();
				return true;
			}
			if(&mine.front() == &theirs.front())
				// if they're already borrowing my memory, then force them to steal my memory
				return that.steal(*this);

			// Otherwise delete own memory and use their buffer
			bufferLock.lock();
			delete[] raw;
			bufferLock.unlock();
			ownMemory = false;
			raw = &theirs.front();
			return true;
		}

	private:
		// Raw buffer
		S* raw;
		// Number of samples in single chunk
		int N_samples;
		// Number of N_samples chunks in buffer
		int N_chunks;
		// Number of samples to skip
		int N_zeroes = 0;
		// Boolean to indicate if object owns memory
		bool ownMemory = true;

		std::mutex bufferLock;

/****************************************************
* WARNING: (end user should not need to modify this)
* This is the default behavior for resizing buffers
* Users are expected to interact with adjustSamples()
****************************************************/
		bool resizeBuffer(size_t sizeBuffer) {
			if(!ownMemory)
				return false;
			bufferLock.lock();
			delete[] raw;
			raw = new S[sizeBuffer];
			bufferLock.unlock();
			return true;
		}
};
#endif
