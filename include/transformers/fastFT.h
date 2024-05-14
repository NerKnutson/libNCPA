#ifndef FAST_FT_H
#define FAST_FT_H
// Performs fast Fourier transform:
//		N_channels*lengthFT <double> input
//			to N_channels*(lengthFT/2 + 1) std::complex<double> output
//		N_channels*(lengthFT/2 + 1) <std::complex<double> input
//			to N_channels*lengthFT <double> output
//
// Typical Operation:
// fastFT<double, std::complex<double>> X(<params>);
// while(X.read(<FILE* or istream>)) {
//		bool check = X.transform();
//		if(check)
//			X.write(<FILE* or ostream>);
// }

#include "TRANSFORMER.h"
#include <complex>
#include <fftw3.h>

// FFT and inverse FFT of N_channels of lengthFT of real data
// S refers to the input <double> or <std::complex<double>>
template <class S = double, class T = std::complex<double>>
class fastFT : public transformer<S,T> {
	public:
		fastFT(size_t N_channels, size_t lengthFT):
			transformer<S,T>(N_channels) {
			plan = selectPlan(*this, N_channels, lengthFT);
		}
		~fastFT() {
			fftw_destroy_plan(plan);
		}

		bool transform() {
			this->bufferLock.lock();
			fftw_execute(plan);
			this->bufferLock.unlock();
			return true;
		}

	private:
		fftw_plan plan;

		fftw_plan selectPlan(fastFT<double,std::complex<double>> &selection, int N_channels, int lengthFT) {
			int N_bins = lengthFT/2 + 1;
			this->input.adjustSamples(N_channels*lengthFT,1);
			this->output.adjustSamples(N_channels*N_bins,1);
			auto realData = this->input.getData();
			auto complexData = this->output.getData();
			return fftw_plan_many_dft_r2c(1, &lengthFT, N_channels,
						(double*) &realData[0], NULL,
						N_channels, 1,
						(double (*)[2]) &complexData[0], NULL,
						N_channels, 1,
						FFTW_ESTIMATE
					);
		}
		fftw_plan selectPlan(fastFT<std::complex<double>,double> &selection, int N_channels, int lengthFT) {
			int N_bins = lengthFT/2 + 1;
			this->input.adjustSamples(N_channels*N_bins,1);
			this->output.adjustSamples(N_channels*lengthFT,1);
			auto complexData = this->input.getData();
			auto realData = this->output.getData();
			return fftw_plan_many_dft_c2r(1, &lengthFT, N_channels,
						(double (*)[2]) &complexData[0], NULL,
						N_channels, 1,
						(double*) &realData[0], NULL,
						N_channels, 1,
						FFTW_ESTIMATE
					);
		}
};

#endif
