#ifndef XCORR_H
#define XCORR_H

#include "fft.h"
#include "ifft.h"

#include <mutex>
#include <thread>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex_math.h>

// XCORR N_chan of real data
// Raw input array needs to be of size N_chan x 2*N_samples_input
// Zero-padding must be done before XCORR_all() is called.

class XCORRer {
	public:
		XCORRer(gsl_vector* input, int N_chan, gsl_vector* output):
			N_chan(N_chan) {
				int N_corr = N_chan*(N_chan-1)/2;
				int N_fft = input->size/N_chan;
				int N_bin = N_fft/2 + 1;

				if(input == NULL) return; // exits if no input array is passed
				raw_input = input;
				_input = new gsl_vector_view[N_chan];
				for(int c = 0; c < N_chan; ++c)
					_input[c] = gsl_vector_subvector_with_stride(raw_input, c, N_chan, N_fft);

				if(output == NULL) { // creates output array if none is passed
					raw_output = gsl_vector_calloc(N_corr*N_fft);
					Q_handle_output = true;
				} else {
					raw_output= output;
					Q_handle_output = false;
				}
				_output = new gsl_vector_view[N_corr];
				for(int c = 0; c < N_corr; ++c)
					_output[c] = gsl_vector_subvector_with_stride(raw_output, c, N_corr, N_fft);

				fft = new FFTer(raw_input, N_chan, N_fft, NULL);

				inverse_fft_input = gsl_vector_complex_calloc(N_corr*N_bin);
				ifft = new IFFTer(inverse_fft_input, N_corr, N_fft, raw_output);
			}

		~XCORRer() {
			gsl_vector_complex_free(inverse_fft_input);
			if(Q_handle_output)
				gsl_vector_free(raw_output);

			delete[] _input;
			delete[] _output;

			delete fft;
			delete ifft;
		}

		bool XCORR_all();

		gsl_vector_view* _input;
		gsl_vector_view* _output;

		gsl_vector* raw_input;
		gsl_vector* raw_output;

		bool XCORR(int chan_a, int chan_b);

		int pair_num(int chan_a, int chan_b) {
			return (2*chan_a*N_chan - chan_a*chan_a + 2*chan_b - 3*chan_a - 2)/2;
		}
	private:
		void conjugate_multiply(gsl_vector_complex* input_a, gsl_vector_complex* input_b, gsl_vector_complex* output);

		FFTer* fft;
		IFFTer* ifft;

		gsl_vector_complex* inverse_fft_input;

		const int N_chan;

		std::mutex buffer_lock;
		bool Q_handle_output;
};
#endif
