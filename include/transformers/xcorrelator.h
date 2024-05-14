#ifndef XCORRELATOR_H
#define XCORRELATOR_H
// Cross-correlates multi-channel data
//		N_channels*lengthFFT/2 input
//			to N_correlations*lengthFFT output
//
// Typical Operation:
// xcorrelator X(<params>);
// while(X.read(<FILE* or istream>)) {
//		bool check = X.transform();
//		if(check)
//			X.write(<FILE* or ostream>);
// }

#include "fastFT.h"
#include "buffers/inputBuffer.h"
#include "buffers/outputBuffer.h"

template <class S = double>
class xcorrelator {
	public:
		xcorrelator(int N_channel, int lengthFFT):
			N_channel(N_channel) {
				int N_correlation = N_channel*(N_channel-1)/2;
				//int N_bin = lengthFFT/2 + 1;
				FFT = new fastFT<S, std::complex<S>>(N_channel, lengthFFT);
				FFT->input.zeroPad(N_channel*lengthFFT/2);
				iFFT = new fastFT<std::complex<S>, S>(N_correlation, lengthFFT);
				input = &FFT->input;
				output = &iFFT->output;
			}

		~xcorrelator() {
			delete[] FFT;
			delete[] iFFT;
		}

		bool transform() {
			int N_correlation = N_channel*(N_channel-1)/2;
			if(!FFT->transform())
				return false; // Fail if unable to FFT
			auto fftBuffer = FFT->output.getData();
			auto ifftBuffer = iFFT->input.getData();

			int N_bins = fftBuffer.size()/N_channel/2 + 1;
			this->bufferLock.lock();
			for(int b = 0; b < N_bins; ++b) {
				auto fftBin = std::span(&fftBuffer[b*N_channel], N_channel);
				auto ifftBin = std::span(&ifftBuffer[b*N_correlation], N_channel);
				int pairNum = 0;
				for(int c = 0; c < N_channel; ++c) {
					for(int d = c+1; d < N_channel; ++d) {
						ifftBin[pairNum] = fftBin[c] * std::conj(fftBin[d]);
						pairNum++;
					}
				}
			}
			this->bufferLock.unlock();
			if(!iFFT->transform())
				return false; // Fail if unable to iFFT
			return true;
		}
		template<class U>
		int read(U &fileInput) { return FFT->read(fileInput); }
		template<class U>
		int write(U &fileOutput) { return iFFT->write(fileOutput); }

		int pair_num(int chan_a, int chan_b) {
			return (2*chan_a*N_channel - chan_a*chan_a + 2*chan_b - 3*chan_a - 2)/2;
		}
		inputBuffer<S>* input;
		outputBuffer<S>* output;
	private:
		fastFT<double, std::complex<double>>* FFT;
		fastFT<std::complex<double>, double>* iFFT;
		const int N_channel;

	protected:
		std::mutex bufferLock;
};
#endif
