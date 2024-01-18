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
					inputBuffer = new S[N_channel*(lengthFT + 1)]
					std::fill(inputBuffer, inputBuffer + N_channel*(lengthFT + 1), 0.0);
					headinputBuffer[0] = inputBuffer + N_channel*lengthFT;
					tailinputBuffer[0] = inputBuffer;

					// 2nd Order Filter
					// Newest, 2nd Newest
					outputBuffer = new T[2*N_channel];
					std::fill(outputBuffer, outputBuffer + 2*N_channel, 0.0);

					headOutputBuffer = outputBuffer;
					tailOutputBuffer = outputBuffer + N_channel;

					const double arg = 2.0 * M_PI * indexBin/lengthFT;
					const std::complex<double> j(0.0, 1.0);
					coeffInput = exp(j * arg);
					coeffOutput= 2 * cos(arg);
				}
		}
		~rollingDFT() {
			delete inputBuffer;
			delete outputBuffer;
		}

		// Expect input[N_channel]
		T* DFT(const S* input) {
			// Overwrite oldest N_channel samples with  most recent
			std::copy(input, input + N_channel, tailInput[0]);
			// Redirect headInput and tailInput appropriately
			headInput[1] = headInput[0];
			headInput[0] = tailInput[0];
			tailInput[0] = tailInput[1];
			if(tailInput[1] == input + N_channel*lengthFT)
				tailInput[1] = input;
			else
				tailInput[1] += N_channel;

			for(int c = 0; c < N_channel; ++c)
				tailOutput[c] = calculateDFT(c);

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
		S* inputBuffer;
		S* headInputBuffer[2]; // {Newest, 2nd Newest}
		S* tailInputBuffer[2]; // {Oldest, 2nd Oldest}

		// 2nd Order Filter: each channel needs 2 points of historical data
		T* outputBuffer;
		T* headOutputBuffer;
		T* tailOutputBuffer;

		std::complex<double> coeffInput;
		double coeffOutput;

		T calculateDFT(const unsigned channel, const S* headInput1, const S* headInput2, const S* tailInput1, const S* tailInput2) {
			// Calculate updated SDFT value
			T value = coeffOutput * headOutputBuffer[channel] - tailOutputBuffer[channel];

			value += coeffInput * (headInput1[channel] - tailInput2[channel]);

			value +=  tailInput1[channel] - headInput2[channel];

			return value;
		}
};
#endif
