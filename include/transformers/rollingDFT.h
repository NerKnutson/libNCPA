#ifndef ROLLING_DFT_H
#define ROLLING_DFT_H
// Performs rolling Fourier transform:
//		N_channels input
//			to N_channels*(N_bins) output
//
// Typical Operation:
// rollingDFT<double, std::complex<double>> X(<params>);
// while(X.read(<FILE* or istream>)) {
//		bool check = X.transform();
//		if(check)
//			X.write(<FILE* or ostream>);
// }

#include <cmath> // exp() cos()
#include <complex> // std::complex<double>
#include "TRANSFORMER.h"
#include <vector> // std::vector<>
#ifdef DEBUG
#include <iostream> // std::cerr
#endif
// It stores data from 0 samples in the past to lengthFT + 1 samples in the past
// Note: This is lengthFT + 2 samples

template <class S = double, class T = std::complex<S>>
class rollingDFT : public transformer<S,T> {
	public:
		// Constructor
		rollingDFT(unsigned N_channels, unsigned lengthFT, std::span<unsigned> listBins):
			transformer<S,T>(N_channels, N_channels * listBins.size(), lengthFT + 2, 3) {
			for(const auto& bin : listBins) {
				if(2 * bin > lengthFT) {
#ifdef DEBUG
					std::cerr << "[rollingDFT] Error: desired frequency violates Nyquist-Shannon theorem." << std::endl;
#endif
					exit(1);
				}
				const double arg = 2.0 * M_PI * bin/lengthFT;
				const std::complex<double> j(0.0, 1.0);
				coeff_I.emplace_back(exp(j * arg));
				coeff_O.emplace_back(2 * cos(arg));
			}
		}
		~rollingDFT() {
			coeff_I.clear();
			coeff_O.clear();
		}
		bool transform() {
			auto head_I = this->input.getData();
			auto head_II = this->input.getData(-1);
			auto tail_I = this->input.getData(1);
			auto tail_II = this->input.getData(2);

			auto head_O = this->output.getData(); // Newest data
			auto head_OO = this->output.getData(-1);
			auto head_OOO = this->output.getData(-2);

			size_t N_channels = head_I.size();
			size_t N_bins = coeff_I.size();
			this->bufferLock.lock();
			for(size_t b = 0; b < N_bins; ++b) {
				auto subHead_O = std::span(&head_O[N_channels * b], N_channels);
				auto subHead_OO = std::span(&head_OO[N_channels * b], N_channels);
				auto subHead_OOO = std::span(&head_OOO[N_channels * b], N_channels);
				for(size_t c = 0; c < N_channels; ++c) {
					subHead_O[c] = coeff_O[b] * subHead_OO[c] - subHead_OOO[c];
					subHead_O[c] += coeff_I[b] * (head_I[c] - tail_II[c]);
					subHead_O[c] += tail_I[c] - head_II[c];
				}
			}
			this->bufferLock.unlock();
			return true;
		}
	private:
		std::vector<std::complex<double>> coeff_I;
		std::vector<double> coeff_O;
};
#endif
