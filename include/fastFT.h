#ifndef fastFT_H
#define fastFT_H
// FFT(double freshInput[]) receives <lengthFT> frame of data from <N_channel> channels
// FFT() performs an FFT on the currently buffered data

#include <mutex>
#include <thread>
#include <complex>
#include <fftw3.h>
#include <algorithm> //std::transform()

// FFT and inverse FFT of N_channel of lengthFT of real data
// S refers to the input <double> or <std::complex<double>>
template <class S>
class fastFT {
	public:
		fastFT(int N_channel, int lengthFT):
			N_channel(N_channel),
			lengthFT(lengthFT) {
			int N_bin = lengthFT/2 + 1;

			realData = (double*) fftw_alloc_real(lengthFT*N_channel);
			complexData = (std::complex<double>*) fftw_alloc_complex(N_bin*N_channel);

			std::fill(realData, realData + lengthFT*N_channel, 0.0);
			std::fill(complexData, complexData + N_bin*N_channel, 0.0);

			plan = selectPlan(this);
		}
		~fastFT() {
			fftw_free(realData);
			fftw_free(complexData);
			fftw_destroy_plan(plan);
		}

		void FFT() {
			buffer_lock.lock();
			fftw_execute(plan);
			buffer_lock.unlock();
		}

		double* realData;
		std::complex<double>* complexData;

	private:
		fftw_plan plan;

		fftw_plan selectPlan(fastFT<double>* selection) {
			return fftw_plan_many_dft_r2c(1, &lengthFT, N_channel,
						realData, NULL,
						N_channel, 1,
						(double (*)[2]) complexData, NULL,
						N_channel, 1,
						FFTW_ESTIMATE
					);
		}
		fftw_plan selectPlan(fastFT<std::complex<double>>* selection) {
			return fftw_plan_many_dft_c2r(1, &lengthFT, N_channel,
						(double (*)[2]) complexData, NULL,
						N_channel, 1,
						realData, NULL,
						N_channel, 1,
						FFTW_ESTIMATE
					);
		}

		const int N_channel;
		const int lengthFT;

		std::mutex buffer_lock;
};

#endif
