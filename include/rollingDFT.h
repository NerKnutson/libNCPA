#ifndef rollingDFT_H
#define rollingDFT_H

#include <cmath> // exp() cos()
#include <complex> // std::complex<double>
#ifdef DEBUG
#include <iostream> // std::cerr
#endif
// Note: S* input is an array of size (lengthFT + 1) * N_channel
// It stores data from 1 sample in the past to lengthFT samples in the past

template <class S = double, class T = std::complex<double>>
class rollingDFT {
	public:
		rollingDFT(unsigned N_channel, unsigned lengthFT, unsigned indexBin):
			N_channel(N_channel),
			lengthFT(lengthFT) {
				if(2 * indexBin > lengthFT) {
#ifdef DEBUG
					std::cerr << "[rollingDFT] Error: desired frequency violates Nyquist-Shannon theorem." << std::endl;
#endif
					return;
				} else {
					output = new T[2*N_channel];
					std::fill(output, output + 2*N_channel, 0.0);

					headOutput = output;
					tailOutput = output + N_channel;

					const double arg = 2.0 * M_PI * indexBin/lengthFT;
					const std::complex<double> j(0.0, 1.0);
					coeffInput = exp(j * arg);
					coeffOutput = 2 * cos(arg);
				}
			}
		~rollingDFT() {
				delete output;
			}
		T* DFT(const S* input, const unsigned indexHead, const unsigned lengthInput) {
		//T* DFT(const S* headInput1, const S* headInput2, const S* tailInput1, const S* tailInput2) {

			//for(int c = 0; c < N_channel; ++c)
				//tailOutput[c] = calculateDFT(c, headInput1, headInput2, tailInput1, tailInput2);
			for(int c = 0; c < N_channel; ++c)
				tailOutput[c] = calculateDFT(c,
					/* Newest */	&input[N_channel*indexHead],
				/* 2nd Newest */	&input[N_channel*getIndexInRange(indexHead - 1, lengthInput)],
					/* Oldest */	&input[N_channel*getIndexInRange(indexHead - lengthFT - 1, lengthInput)],
				/* 2nd Oldest */	&input[N_channel*getIndexInRange(indexHead - lengthFT, lengthInput)]
									);

			// Update head to newest output sample
			if(headOutput == output) {
				headOutput = tailOutput;
				tailOutput = output;
			}
			else {
				tailOutput = headOutput;
				headOutput = output;
			}
			return headOutput;
		}
	private:
		unsigned N_channel;
		unsigned lengthFT;

		// 2nd Order Filter: each channel needs 2 points of historical data
		T* output;
		T* headOutput;
		T* tailOutput;

		std::complex<double> coeffInput;
		double coeffOutput;

		T calculateDFT(const unsigned channel, const S* headInput1, const S* headInput2, const S* tailInput1, const S* tailInput2) {
			// Calculate updated SDFT value
			T value = coeffOutput * headOutput[channel] - tailOutput[channel];

			value += coeffInput * (headInput1[channel] - tailInput2[channel]);

			value +=  tailInput1[channel] - headInput2[channel];

			return value;
		}
		unsigned getIndexInRange(const int index, const unsigned lengthBuffer) {
			return (index + lengthBuffer) % lengthBuffer;
		}
};
#endif
