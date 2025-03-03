#include "generators/functionGenerator.h"

#include <cstring>
using namespace std;

int main(int argc, char* argv[]) {
	// expected input is num_samples, time_step, time_offset, {array of terms}
	int num_samples = 10;
	double time_step = 1;
	double time_offset = 0;

	vector<functionTerm> vec({});
	for (int arg = 0; arg < argc; ++arg) {
		if (strcmp(argv[arg], "--num_samples") == 0) {
			//set num_samples
			if (!sscanf(argv[arg + 1], "%d", &num_samples)) {
				fprintf(stderr, "%s: failed to parse num_samples arguement: '%s %s'\n", argv[0], argv[arg], argv[arg + 1]);
				return 1;
			}
			if (num_samples <= 0) {
				fprintf(stderr, "%s: num_samples cant be less than 1, currently %d\n", argv[0], num_samples);
				num_samples = 100;
			}
		}
		else if (strcmp(argv[arg], "--time_step") == 0) {
			//set time_step
			if (!sscanf(argv[arg + 1], "%lf", &time_step)) {
				fprintf(stderr, "%s: failed to parse time_step arguement: '%s %s'\n", argv[0], argv[arg], argv[arg + 1]);
				return 1;
			}
		}
		else if (strcmp(argv[arg], "--time_offset") == 0) {
			//set time_offset
			if (!sscanf(argv[arg + 1], "%lf", &time_offset)) {
				fprintf(stderr, "%s: failed to parse time_offset arguement: '%s %s'\n", argv[0], argv[arg], argv[arg + 1]);
				return 1;
			}
		}
		else if (strcmp(argv[arg], "-h") == 0 || strcmp(argv[arg], "--help") == 0) {
			cout << "expected use: " << argv[0] << " --num_samples {int} [default 10] --time_step {double} [default 1] --time_offset {double} [default 0] {array of terms, like sin amp,coef,offset,power or quad coef,offset,power}\n";
		}
		else if (strcmp(argv[arg], "sin") == 0) {
			double amplitude;
			double coefficient;
			double offset;
			double power;
			if (!sscanf(argv[arg + 1], "%lf,%lf,%lf,%lf", &amplitude, &coefficient, &offset, &power)) {
				fprintf(stderr, "%s: failed to parse sin argumenet: '%s %s'", argv[0], argv[arg], argv[arg + 1]);
			}
			functionTerm newTerm = functionTerm("sin", amplitude, coefficient, offset, power);
			vec.insert(vec.end(), newTerm);
		}
		else if (strcmp(argv[arg], "cos") == 0) {
			double amplitude;
			double coefficient;
			double offset;
			double power;
			if (!sscanf(argv[arg + 1], "%lf,%lf,%lf,%lf", &amplitude, &coefficient, &offset, &power)) {
				fprintf(stderr, "%s: failed to parse cos argumenet: '%s %s'", argv[0], argv[arg], argv[arg + 1]);
			}
			functionTerm newTerm = functionTerm("cos", amplitude, coefficient, offset, power);
			vec.insert(vec.end(), newTerm);
		}
		else if (strcmp(argv[arg], "quad") == 0) {
			double coefficient;
			double offset;
			double power;
			if (!sscanf(argv[arg + 1], "%lf,%lf,%lf", &coefficient, &offset, &power)) {
				fprintf(stderr, "%s: failed to parse quad argumenet: '%s %s'", argv[0], argv[arg], argv[arg + 1]);
			}
			functionTerm newTerm = functionTerm("quad", coefficient, offset, power);
			vec.insert(vec.end(), newTerm);
		}
	}

	functionGenerator<double> generator(num_samples, time_step, time_offset, vec);
	double current_time = time_offset;
	for (double i = 0; i < num_samples; i++) {
		cout << "@ " << current_time << ", : ";
		for (auto term : vec) {
			cout << term.evaluate(current_time) << " + ";
		}
		cout << "0 = " << generator.generateSingle(current_time) << "\n";
		current_time += time_step;
	}
	return 1;
}