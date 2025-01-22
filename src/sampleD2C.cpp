#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/distanceToCoordinate.h" // distanceToCoordinate(int,int)

#include <eigen3/Eigen/Core>
#include <vector>
#include "mds.h"

using namespace Eigen;
using namespace std;

/*
	A sample program to demonstrate the flow from geometry buffers holding distances through D2C to a geometry buffer holding coordinates
*/

int main(int argc, char* argv[]) {
	/*if (argc != 2) {
		cerr << "Usage: " << argv[0] << "" << endl;
		return 1;
	}*/
	int N_channels = 9;
	int N_dimensions = 3;

	geometryBuffer<double> initialPoints(N_channels, N_dimensions);
	double initialPointsData[N_channels][N_dimensions] = { 
		{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
		{-1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, -1.0},
		{1.0, 1.0, 1.0},
		{-1.0, -1.0, -1.0}
	};

	cout << "Initial Point Matrix " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_dimensions; j++) {
			cout << initialPointsData[i][j] << " ";
		}
		cout << endl;
	}
	// 2-dimensional data for 5 points
	//std::span<double> initialPointsSpan(initialPointsData);

	double initialDistancesData[N_channels][N_channels];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			initialDistancesData[i][j] = sqrt( pow((initialPointsData[j][0] - initialPointsData[i][0]), 2) + pow((initialPointsData[j][1] - initialPointsData[i][1]), 2) + pow((initialPointsData[j][2] - initialPointsData[i][2]), 2) );
		}
	}
	cout << endl << "Initial Distance Matrix " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			//cout << initialDistancesData[i][j] << " ";
			printf("%.2f ", initialDistancesData[i][j]);
		}
		cout << endl;
	}
	std::span<double> initialDistancesSpan(initialDistancesData[0], N_channels*N_channels);
	//geometryBuffer<double> distances(N_channels, N_channels); //n by n matrix of distances between n nodes
	//distances.read(initialDistancesSpan);
	geometryBuffer<double> coordinates(N_channels, N_dimensions); //n by 2 matrix of coordinates

	distanceToCoordinate<double> D2C(N_channels, N_dimensions);

	/*while (D2C.read(std::cin)) {
		if (!D2C.transform()) {
			cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
			return 1;
		}
		if (D2C.write(std::cout) < 0) {
			cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
			return 1;
		}
	}*/
	D2C.read(initialDistancesSpan);

	if (!D2C.transform()) {
		cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
		return 1;
	}
	/*if (D2C.write(std::cout) < 0) {
		cerr << "Error in " << argv[0] << ":\n failed to write." << endl;
		return 1;
	}*/
	double expectedPoints[N_channels][N_dimensions];
	std::span<double> expectedPointsData = D2C.output.getData();
	cout << endl << "Expected Points Data " << endl;;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_dimensions; j++) {
			cout << expectedPointsData[j + i*N_dimensions] << "\t";
			expectedPoints[i][j] = expectedPointsData[j + i * N_dimensions];
		}
		cout << endl;
	}

	double expectedDistancesData[N_channels][N_channels];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			expectedDistancesData[i][j] = sqrt(pow((expectedPoints[j][0] - expectedPoints[i][0]), 2) + pow((expectedPoints[j][1] - expectedPoints[i][1]), 2) + pow((expectedPoints[j][2] - expectedPoints[i][2]), 2));
		}
	}
	double scalingcoef = initialDistancesData[0][N_channels-1] / expectedDistancesData[0][N_channels - 1];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			expectedDistancesData[i][j] *= scalingcoef;
		}
	}
	cout << endl << "Expected Distance Matrix " << "(scaling coef: " << scalingcoef << ")" << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			//cout << expectedDistancesData[i][j] << " ";
			printf("%.2f ", expectedDistancesData[i][j]);
		}
		cout << endl;
	}

	cout << endl << "Total squared error: ";
	double squaresum = 0;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			squaresum += pow(expectedDistancesData[i][j] - initialDistancesData[i][j], 2);
		}
	}
	cout << squaresum << endl;


	cout << endl << "v2: " << endl;
	MatrixXd D(N_channels, N_channels);
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			D(i, j) = sqrt(pow((initialPointsData[j][0] - initialPointsData[i][0]), 2) + pow((initialPointsData[j][1] - initialPointsData[i][1]), 2) + pow((initialPointsData[j][2] - initialPointsData[i][2]), 2));
		}
	}

	// Compute MDS (embedding into a 3-dimensional space)
	const MatrixXd X = MDS::computeMDS(D, 3).transpose();

	// Show the result
	//cout << X << endl;
	cout << endl << "Expected Points Data 2 : " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_dimensions; j++) {
			cout << X(i, j)<< "\t";
		}
		cout << endl;
	}

	double expectedDistancesData2[N_channels][N_channels];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			expectedDistancesData2[i][j] = sqrt(pow((X(j, 0) - X(i, 0)), 2) + pow((X(j, 1) - X(i, 1)), 2) + pow((X(j, 2) - X(i, 2)), 2));
		}
	}
	double scalingcoef2 = initialDistancesData[0][N_channels - 1] / expectedDistancesData2[0][N_channels - 1];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			expectedDistancesData2[i][j] *= scalingcoef2;
		}
	}
	cout << endl << "Expected Distance Matrix " << "(scaling coef: " << scalingcoef2 << ")" << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			//cout << expectedDistancesData[i][j] << " ";
			printf("%.2f ", expectedDistancesData2[i][j]);
		}
		cout << endl;
	}

	cout << endl << "Total squared error: ";
	double squaresum2 = 0;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			squaresum2 += pow(expectedDistancesData2[i][j] - initialDistancesData[i][j], 2);
		}
	}
	cout << squaresum2 << endl;


	return 0;
}
