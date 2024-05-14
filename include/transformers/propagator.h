#ifndef PROPAGATOR_H
#define PROPAGATOR_H
#include "TRANSFORMER.h"

template <class S = double, class T = double>
class propagator : public transformer<S,T> {
	public:
		template<class V, class W>
		propagator(size_t N_sources, size_t N_channels, size_t N_dimensions, double sampleRate, V& geometryInput, W& slownessInput):
		transformer<S,T>(N_sources, N_channels) {
			arrayGeometry.adjustSamples(N_channels, N_dimensions);
			arrayGeometry.read(fileInput); // Read in array geometry coordinates
			slownessVectors.adjustSamples(N_sources, N_dimensions);
			slownessVectors.read(slownessInput);
		}
		~propagator() { }
		int transform() {
			auto inputData = this->input.getData(); // data to propagate
			auto newData = this->output.getData(); // propagated data
			this->bufferLock.lock();
			for(int c = 0; c < newData.size(); ++c)
				newData[c] = bandPass[c].filter(dist(generator));
			this->bufferLock.unlock();
			return newData.size()*sizeof(S);
		}
		geometryBuffer<T> arrayGeometry;
		geometryBuffer<T> slownessVectors;
	private:
		double calcMaxDistance() {
		}
		void calcSampleDelays(double sampleDelays[], double arrayGeometry[][3], double slowness[3], unsigned N_channels, double sampleFrequency) {
			double distanceMax = -1;
			auto geometry = arrayGeometry.getData();
			auto channel = arrayGeometry.getData(0);
			const int N_channels = geometry.size()/channel.size();
			const int N_dimensions = channel.size();
			for(int c = 1; c < N_channels; ++c) {
				auto channel_A = arrayGeometry.getData(c);
				double tmp = 0;
				for(int d = 0; d < N_dimensions; ++d) {
					double dist = channel[d] - channel_A[d];
					tmp += dist*dist;
				}
				if(tmp > distanceMax)
					distanceMax = tmp;
			}
			return distanceMax;


			int maxDelay = -INT_MAX;
			int minDelay = INT_MAX;
			for(int c = 0; c < N_channels; ++c) {
				sampleDelays[c] = 0;
				for(int d = 0 ; d < 3; ++d) {
					sampleDelays[c] -= arrayGeometry[c][d] * slowness[d] * sampleFrequency;
				}
				if(sampleDelays[c] > maxDelay)
					maxDelay = sampleDelays[c];
				if(sampleDelays[c] < minDelay)
					minDelay = sampleDelays[c];
			}
			for(int c = 0; c < N_channels; ++c) {
				sampleDelays[c] += abs(minDelay) + 1;
			}
		}
		// returns sample frequency
		double calcSlownessVector(FILE* fileName, double slowness[3]) {
			double sampleFrequency;
			double slownessCheck = 0;
			if(fileName == NULL) {
				cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal source filename" << endl;
				return 1;
			} else {
				fflush(fileName);
				rewind(fileName);
				fscanf(fileName, "sampleFrequency = %lf\n", &sampleFrequency);
				if(sampleFrequency <= 0.0) {
					cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal sample frequency" << endl;
					return 1;
				}
				double speed = 0.0;
				fscanf(fileName, "speed = %lf\n", &speed);
				if(speed <= 0.0) {
					cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal speed" << endl;
					return 1;
				}
				double azimuth = 0.0;
				double inclination = 0.0;
				fscanf(fileName, "azimuth = %lf\n", &azimuth);
				fscanf(fileName, "inclination = %lf\n", &inclination);

				azimuth *= M_PI/180.0;
				inclination *= M_PI/180.0;
				slowness[0] = cos(azimuth) * cos(inclination) / speed;
				slowness[1] = sin(azimuth) * cos(inclination) / speed;
				slowness[2] = sin(inclination) / speed;
				for(int d = 0 ; d < 3; ++d) {
					slownessCheck += slowness[d] * slowness[d];
				}
				if(slownessCheck <= 0.0) {
					cerr << "Error in calcSlownessVector(FILE*,double[3]):\nInvalid signal direction" << endl;
					return 1;
				}
			}
			return sampleFrequency;
		}
};
#endif
