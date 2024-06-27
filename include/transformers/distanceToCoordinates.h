#ifndef d2rCalculator_H
#define d2rCalculator_H
// Decimates data stream:
//		N_channels*decimationFactor input
//			to N_channels output
//
// Typical Operation:
// decimator D(<params>);
// while(D.read(<FILE* or istream>)) {
//		bool check = D.transform();
//		if(check)
//			D.write(<FILE* or ostream>);
// }
//
// TODO: Reads in distance matrix
// Transforms to coordinates

#include "TRANSFORMER.h"
#include "buffers/geometryBuffer.h"
#include <armadillo> // eig_sym(eig_val,eig_vec,distance_matrix)

template <class S = double>
class distanceToCoordinate : public transformer<S,double> {
	public:
		distanceToCoordinate(size_t numberChannels, size_t dimensions) :
		N_channels(numberChannels), N_dimensions(dimensions),
		transformer<S,double>(numberChannels*numberChannels, numberChannels*dimensions),
		coordinates(N_channels, dimensions) {
			coordinates.adjustSamples(dimensions, N_channels);
			coordinates.steal(this->output);
		}

		~distanceToCoordinate() { }

		bool transform() {
			auto head_I = this->input.getBuffer();
			auto head_O = this->output.getBuffer(); // Newest data

			// Set up matrix access
			distanceMatrix = arma::mat(&head_I[0], N_channels, N_channels, false, true);
			arma::colvec onesCol(N_channels,arma::fill::ones);
			arma::rowvec onesRow(N_channels,arma::fill::ones);

			arma::vec eigenValues;
			arma::mat eigenVectors;
			this->bufferLock.lock();
			arma::mat gramianMatrix = 0.5*(onesCol * distanceMatrix.row(0) + distanceMatrix.col(0) * onesRow - distanceMatrix);
			this->bufferLock.unlock();
			arma::eig_sym(eigenValues, eigenVectors, gramianMatrix);
			eigenValues = eigenValues.tail(3);
			eigenVectors = eigenVectors.tail_cols(3);
			eigenVectors = arma::normalise(eigenVectors, 2, 0);

			coordinateMatrix = arma::trans(eigenVectors*sqrt(arma::diagmat(eigenValues)));
			coordinates.read(coordinateMatrix);
			std::copy(coordinateMatrix.begin(), coordinateMatrix.end(), head_O.begin());
			return true;
		}

		geometryBuffer<double> coordinates;
	private:
		arma::mat distanceMatrix;
		arma::mat coordinateMatrix;
		const int N_channels;
		const int N_dimensions;
};
#endif
