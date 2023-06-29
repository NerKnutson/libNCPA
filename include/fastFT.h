#ifndef fastFT_H
#define fastFT_H

#include <mutex>
#include <thread>
#include <complex>
#include <fftw3.h>
#include <algorithm> //std::transform()

// FFT N_channel of real data

template <class S, class T>
class fastFT {
	public:
		fastFT(int N_channel, int lengthFT):
			N_channel(N_channel),
			lengthFT(lengthFT) {
			fastFT(this, N_channel, lengthFT);
		}
		~fastFT() {
			fftw_destroy_plan(bigPlan);

			fftw_free(rawInput);
			fftw_free(rawOutput);
		}

		template<typename T1>
		bool convertData(const T1& data) {
			std::transform(data, data + N_channel * lengthFT, rawInput, static_cast_func());
			return true;
		}

		bool FFT() {
			buffer_lock.lock();
			fftw_execute(bigPlan);
			buffer_lock.unlock();
			return true;
		}
		bool FFT(double freshInput[]) {
			memcpy(rawInput, freshInput, N_channel * lengthFT * sizeof(double));
			buffer_lock.lock();
			fftw_execute(bigPlan);
			buffer_lock.unlock();
			return true;
		}
		bool FFT(std::complex<double> freshInput[]) {
			memcpy(rawInput, freshInput, N_channel * (lengthFT/2 + 1) * sizeof(std::complex<double>));
			buffer_lock.lock();
			fftw_execute(bigPlan);
			buffer_lock.unlock();
			return true;
		}
		bool FFT(int freshInput[]) {
			convertData(freshInput);
			buffer_lock.lock();
			fftw_execute(bigPlan);
			buffer_lock.unlock();
			return true;
		}
		bool FFT(std::complex<int> freshInput[]) {
			convertData(freshInput);
			buffer_lock.lock();
			fftw_execute(bigPlan);
			buffer_lock.unlock();
			return true;
		}

		S* rawInput;
		T* rawOutput;

	private:
		fftw_plan bigPlan;

		const int N_channel;
		const int lengthFT;

		std::mutex buffer_lock;
		bool Q_handle_output;

		fastFT(fastFT<double, std::complex<double>>* r2c, int N_channel, int lengthFT):
			N_channel(N_channel),
			lengthFT(lengthFT) {
				int N_bin = lengthFT/2 + 1;

				r2c->rawInput = (S*) fftw_alloc_real(lengthFT*N_channel);
				r2c->rawOutput = (T*) fftw_alloc_complex(N_bin*N_channel);

				r2c->bigPlan
					= fftw_plan_many_dft_r2c(1, &lengthFT, N_channel,
							r2c->rawInput, NULL,
							N_channel, 1,
							(double (*)[2]) r2c->rawOutput, NULL,
							N_channel, 1,
							FFTW_ESTIMATE);
		}
		fastFT(fastFT<std::complex<double>, double>* c2r, int N_channel, int lengthFT):
			N_channel(N_channel),
			lengthFT(lengthFT) {
				int N_bin = lengthFT/2 + 1;

				c2r->rawInput = (S*) fftw_alloc_complex(N_bin*N_channel);
				c2r->rawOutput = (T*) fftw_alloc_real(lengthFT*N_channel);

				c2r->bigPlan
					= fftw_plan_many_dft_c2r(1, &lengthFT, N_channel,
							(double (*)[2]) c2r->rawInput, NULL,
							N_channel, 1,
							c2r->rawOutput, NULL,
							N_channel, 1,
							FFTW_ESTIMATE);
		}

		struct static_cast_func { // use this in the converstion from any type to correct input type
			template <typename T1> S operator()(const T1& x) const{ return static_cast<S>(x); }
		}; //std::transform(begin_input, end_input, output, static_cast_func<TYPE>());
};

#endif
