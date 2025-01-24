#include <iostream> // std::cerr std::cout
#include <cstring> // std::strcmp(char*,char*)
#include "transformers/distanceToCoordinate.h" // distanceToCoordinate(int,int)

using namespace std;

/*
	A sample program to demonstrate the flow from geometry buffers holding distances through D2C to a geometry buffer holding coordinates
*/

int main(int argc, char* argv[]) {
	int N_channels = 9;
	int N_dimensions = 3;

	//Create 9 points to demonstrate D2C and MDS
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

	//Print out the points
	cout << "Initial Point Matrix " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_dimensions; j++) {
			cout << initialPointsData[i][j] << " ";
		}
		cout << endl;
	}

	//Get the distance matrix between points
	double initialDistancesData[N_channels][N_channels];
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			initialDistancesData[i][j] = sqrt( pow((initialPointsData[j][0] - initialPointsData[i][0]), 2) + pow((initialPointsData[j][1] - initialPointsData[i][1]), 2) + pow((initialPointsData[j][2] - initialPointsData[i][2]), 2) );
		}
	}

	//Print out the distance matrix
	cout << endl << "Initial Distance Matrix " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			//cout << initialDistancesData[i][j] << " ";
			printf("%.2f ", initialDistancesData[i][j]);
		}
		cout << endl;
	}

	//Need to create a span to hold the distances, in order to feed into D2C
	std::span<double> initialDistancesSpan(initialDistancesData[0], N_channels*N_channels);

	/*cout << endl << endl << "COMPARING: " << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			cout << initialDistancesSpan[i * N_channels + j] << " " << initialDistancesData[i][j] << endl;
		}
	}
	cout << endl << endl;*/
	//geometryBuffer<double> coordinates(N_channels, N_dimensions); //n by 2 matrix of coordinates

	//Instantiate D2C
	distanceToCoordinate<double> D2C(N_channels, N_dimensions);

	//Feed the data into D2C
	D2C.read(initialDistancesSpan);

	//Run D2C
	if (!D2C.transform()) {
		cerr << "Error in " << argv[0] << ":\n failed to transform." << endl;
		return 1;
	}

	//Get the data out from D2C
	double expectedPoints[N_channels][N_dimensions];
	std::span<double> expectedPointsData = D2C.coordinates.getData();

	//Print out data
	cout << endl << "Expected Points Data " << endl;;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_dimensions; j++) {
			cout << expectedPointsData[j + i*N_dimensions] << "\t";
			expectedPoints[i][j] = expectedPointsData[j + i * N_dimensions];
		}
		cout << endl;
	}

	//Compare results with original distance matrix
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

	//Print out new distance matrix
	cout << endl << "Expected Distance Matrix " << "(scaling coef: " << scalingcoef << ")" << endl;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			//cout << expectedDistancesData[i][j] << " ";
			printf("%.2f ", expectedDistancesData[i][j]);
		}
		cout << endl;
	}

	//Measure accuracy through squared error
	cout << endl << "Total squared error: ";
	double squaresum = 0;
	for (int i = 0; i < N_channels; i++) {
		for (int j = 0; j < N_channels; j++) {
			squaresum += pow(expectedDistancesData[i][j] - initialDistancesData[i][j], 2);
		}
	}
	cout << squaresum << endl;

	return 0;
}
