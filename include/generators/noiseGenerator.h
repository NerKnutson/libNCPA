#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H
#include "GENERATOR.h"

#ifndef ORDER
#define ORDER 3
#endif

template <class S = double>
class noiseGenerator : public generator<S> {
	public:
		noiseGenerator(size_t N_channels, double sampleRate):
		generator<S>(N_channels) {
			bandPass = new Iir::Butterworth::BandPass<ORDER>[N_channels];
			for(auto& elem : bandPass)
				elem.setup(sampleRate, sampleRate/4.0, sampleRate/2.0);
		}
		noiseGenerator(size_t N_channels, double sampleRate, double centerFrequency):
		generator<S>(N_channels) {
			bandPass = new Iir::Butterworth::BandPass<ORDER>[N_channels];
			for(auto& elem : bandPass)
				elem.setup(sampleRate, centerFrequency, centerFrequency/10.0);
		}
		noiseGenerator(size_t N_channels, double sampleRate, double centerFrequency, double widthFrequency):
		generator<S>(N_channels) {
			bandPass = new Iir::Butterworth::BandPass<ORDER>[N_channels];
			for(auto& elem : bandPass)
				elem.setup(sampleRate, centerFrequency, widthFrequency);
		}
		noiseGenerator(size_t N_channels, size_t N_frames, size_t sampleRate):
		generator<S>(N_channels, N_frames) {
			bandPass.setup(sampleFrequency, centerFrequency, widthFrequency);
		}
		~noiseGenerator() { delete[] bandPass; }
		int generate() {
			auto newData = this->output.getData(); // Put data into the next one
			this->bufferLock.lock();
			for(int c = 0; c < newData.size(); ++c)
				newData[c] = bandPass[c].filter(dist(generator));
			this->bufferLock.unlock();
			return newData.size()*sizeof(S);
		}
	private:
		Iir::Butterworth::BandPass<ORDER>* bandPass;
		default_random_engine generator;
		generator.seed(time(NULL));
		normal_distribution<double> dist(0.0, 1.0);
};
#endif
