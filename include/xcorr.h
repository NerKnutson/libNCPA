#ifndef XCORR_H
#define XCORR_H

#include "../include/fastFT.h"

#include <mutex>
#include <thread>

// XCORR N_chan of real data
// Raw input array needs to be of size N_chan x 2*N_samples_input
// Zero-padding must be done before XCORR_all() is called.

class xcorrelator {
	public:
		xcorrelator(int N_channel, int lengthFFT):
			N_channel(N_channel),
			lengthFFT(lengthFFT) {
				int N_correlation = N_channel*(N_channel-1)/2;
				int N_bin = lengthFFT/2 + 1;
				FFT = new fastFT<double, std::complex<double>>(N_channel, lengthFFT);
				iFFT = new fastFT<std::complex<double>, double>(N_channel, lengthFFT);
			}

		~xcorrelator() {
			delete FFT;
			delete iFFT;
		}

		bool xcorrelate() {
			FFT.FFT();
		}

		bool xcorrelate(double freshInput[]) {
			memcpy(FFT.rawInput, freshInput, N_channel * (lengthFT/2 + 1) * sizeof(std::complex<double>));
			FFT.FFT();
			int pairNum = 0;
			for(int b = 0; b < lengthFT/2 + 1; ++b;) {
				for(int c = 0; c < N_channel; ++c) {
					for(int d = c; d < N_channel; ++d) {
						iFFT.rawInput[pairNum] = FFT.rawOutput[c] * std::conj(FFT.rawOutput[d]);
						pairNum++;
					}
				}
			}
		}

		int pair_num(int chan_a, int chan_b) {
			return (2*chan_a*N_chan - chan_a*chan_a + 2*chan_b - 3*chan_a - 2)/2;
		}
	private:
		void conjugate_multiply(gsl_vector_complex* input_a, gsl_vector_complex* input_b, gsl_vector_complex* output);

		fastFT* FFT;
		fastFT* iFFT;

		const int N_channel;
		const int lengthFT;

		std::mutex buffer_lock;
};
#endif
