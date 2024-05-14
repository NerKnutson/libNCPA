#ifndef TIME_STAMPER_H
#define TIME_STAMPER_H
// Takes input of bools and records time stamps
// N_samples input to N_samples output (output is the same buffer)
//
// Typical Operation:
// nullBuffer<<type>> X(<params>);
// while(X.read(<FILE* or istream>)) {
//		X.transform(); // this is not a necessary function call
//		X.write(<FILE* or ostream>);
// }

#include "TRANSFORMER.h"
#include <chrono>

class timeStamper : public transformer<bool,unsigned long> {
	public:
		timeStamper(size_t numberChannels):
		transformer<bool,unsigned long>(numberChannels) {
			const auto now = std::chrono::system_clock::now();
			const auto duration = now.time_since_epoch();
			referenceTick = duration.count();
		}
		bool transform() {
			const auto now = std::chrono::system_clock::now();
			const auto duration = now.time_since_epoch();

			auto head_I = this->input.getData();
			auto head_O = this->output.getData();
			this->bufferLock.lock();
			for(size_t index = 0; index < head_I.size(); ++index) {
				if(head_I[index])
					head_O[index] = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() - referenceTick;
				else
					head_O[index] = 0;
			}
			this->bufferLock.unlock();
			return true;
		}
	private:
		unsigned long referenceTick;
};

#endif
