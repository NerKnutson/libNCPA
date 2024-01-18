#ifndef rollingDFT_H
#define rollingDFT_H

#include <cmath> // exp() cos()
#include <complex> // std::complex<double>
#ifdef DEBUG
#include <iostream> // std::cerr
#endif
// It stores data from 0 samples in the past to lengthFT samples in the past

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
					// N + 1 order filter
					// Newest, 2nd Newest, 2nd Oldest, Oldest
					input = new S[N_channel*(lengthFT + 2)];
					std::fill(input, input + N_channel*(lengthFT + 2), 0.0);
					headInput[0] = input + N_channel*(lengthFT + 2);
					headInput[1] = input + N_channel*lengthFT;
					tailInput[0] = input;
					tailInput[1] = input + N_channel;

					// 2nd Order Filter
					// Newest, 2nd Newest
					output = new T[2*N_channel];
					std::fill(output, output + 2*N_channel, 0.0);

					headOutput = output;
					tailOutput = output + N_channel;

					const double arg = 2.0 * M_PI * indexBin/lengthFT;
					const std::complex<double> j(0.0, 1.0);
					coeffInput = exp(j * arg);
					coeffOutput= 2 * cos(arg);
				}
		}
		~rollingDFT() {
			delete input;
			delete output;
		}

		// Expect data[N_channel]
		T* DFT(const S* data) {
			// Overwrite oldest N_channel samples with  most recent
			std::copy(data, data + N_channel, tailInput[0]);
			// Redirect headInput and tailInput appropriately
			headInput[1] = headInput[0];
			headInput[0] = tailInput[0];
			tailInput[0] = tailInput[1];
			if(tailInput[1] == input + N_channel*lengthFT)
				tailInput[1] = input;
			else
				tailInput[1] += N_channel;

			// Overwrite oldest N_channel output with  most recent
			for(int c = 0; c < N_channel; ++c)
				tailOutput[c] = calculateDFT(c);

			// Update head to newest output sample
			headOutput = tailOutput;
			if(tailOutput == output + N_channel)
				tailOutput = output;
			else
				tailOutput += N_channel;

			return headOutput;
		}
	private:
		unsigned N_channel;
		unsigned lengthFT;

		// 2nd Order Filter: each channel needs 2 points of historical data
		S* input;
		S* headInput[2]; // {Newest, 2nd Newest}
		S* tailInput[2]; // {Oldest, 2nd Oldest}

		// 2nd Order Filter: each channel needs 2 points of historical data
		T* output;
		T* headOutput;
		T* tailOutput;

		std::complex<double> coeffInput;
		double coeffOutput;

		T calculateDFT(const unsigned channel) {
			// Calculate updated SDFT value
			T value = coeffOutput * headOutput[channel] - tailOutput[channel];

			value += coeffInput * (headInput[0][channel] - tailInput[1][channel]);

			value +=  tailInput[0][channel] - headInput[1][channel];

			return value;
		}
};
#endif
