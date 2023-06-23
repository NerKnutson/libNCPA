#include "xcorr.h"

bool XCORRer::XCORR(int chan_a, int chan_b) {
	if(!fft->FFT(chan_a)) return false;
	if(!fft->FFT(chan_b)) return false;

	int pair = pair_num(chan_a, chan_b);
	conjugate_multiply(
			&fft->_output[chan_a].vector,
			&fft->_output[chan_b].vector,
			&ifft->_input[pair].vector
		);

	if(!ifft->IFFT(pair)) return false;

	return true;
}

bool XCORRer::XCORR_all() {
	if(!fft->FFT_all()) return false;

	buffer_lock.lock();
	std::thread Thread[N_chan*(N_chan-1)/2];
	int pair_num = 0;
	for(int c = 0; c < N_chan; ++c) {
		for(int d = c + 1; d < N_chan; ++d) {
			Thread[pair_num++]
				= std::thread(&XCORRer::conjugate_multiply, this,
						&fft->_output[c].vector,
						&fft->_output[d].vector,
						&ifft->_input[pair_num].vector
					);
		}
	}
	pair_num = 0;
	for(int c = 0; c < N_chan; ++c) {
		for(int d = c + 1; d < N_chan; ++d) {
			Thread[pair_num++].join();
		}
	}
	buffer_lock.unlock();

	if(!ifft->IFFT_all()) return false;

	return true;
}

void XCORRer::conjugate_multiply(gsl_vector_complex* input_a, gsl_vector_complex* input_b, gsl_vector_complex* output) {
	for(int e = 0; e < output->size; ++e) {
		gsl_vector_complex_set(
				output,
				e,
				gsl_complex_mul(
						gsl_vector_complex_get(input_a, e),
						gsl_complex_conjugate(
							gsl_vector_complex_get(input_b, e)
							)
					)
			);
	}
}
