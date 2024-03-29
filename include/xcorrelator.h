#ifndef XCORR_H
#define XCORR_H

#include "../include/fastFT.h"

// XCORR N_chan of real data
// Raw input array needs to be of size N_chan x 2*N_samples_input
// Zero-padding must be done before xcorrelate() is called.
// xcorrelate(double freshInput[]) receives <lengthFT> frame of data from <N_channel> channels
// xcorrelate() performs a cross-correlation on the currently buffered data

class xcorrelator {
	public:
		xcorrelator(int N_channel, int lengthFFT):
			N_channel(N_channel),
			lengthFT(lengthFFT) {
				int N_correlation = N_channel*(N_channel-1)/2;
				int N_bin = lengthFT/2 + 1;
				FFT = new fastFT<double>(N_channel, lengthFT);
				iFFT = new fastFT<std::complex<double>>(N_correlation, lengthFT);
				rawInput = FFT->realData;
				rawOutput = iFFT->realData;
			}

		~xcorrelator() {
			delete[] FFT;
			delete[] iFFT;
		}

		bool xcorrelate() {
			int N_correlation = N_channel*(N_channel-1)/2;
			FFT->FFT();
			for(int b = 0; b < lengthFT/2 + 1; ++b) {
				int pairNum = 0;
				for(int c = 0; c < N_channel; ++c) {
					for(int d = c+1; d < N_channel; ++d) {
						iFFT->complexData[pairNum + b*N_correlation] = FFT->complexData[c + b*N_channel] * std::conj(FFT->complexData[d + b*N_channel]);
						pairNum++;
					}
				}
			}
			iFFT->FFT();
			for(int s = 0; s < lengthFT*N_channel * (N_channel-1)/2; ++s)
				rawOutput[s] = rawOutput[s]/(lengthFT * lengthFT);
			return true;
		}

		int pair_num(int chan_a, int chan_b) {
			return (2*chan_a*N_channel - chan_a*chan_a + 2*chan_b - 3*chan_a - 2)/2;
		}

		double* rawInput;
		double* rawOutput;
	private:
		fastFT<double>* FFT;
		fastFT<std::complex<double>>* iFFT;

		const int N_channel;
		const int lengthFT;
};
#endif
