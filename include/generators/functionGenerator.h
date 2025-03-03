#ifndef FUNCTION_GENERATOR_H
#define FUNCTION_GENERATOR_H
#include "GENERATOR.h"
#include <vector>
#include <string>
#include <cmath>
#ifndef ORDER
#define ORDER 3
#endif

# define M_PI           3.14159265358979323846  /* pi */

class functionTerm {
public:
	std::string type;
	double amplitude;
	double coefficient;
	double offset;
	double power;
	functionTerm(std::string type, double amplitude, double coefficient, double offset, double power) {
		// for sin/cos, it is in the form of amplitude * (sin(coefficient * t + offset) ^ power)
		this->type = type; //should be "cos", or "sin"
		this->amplitude = amplitude;
		this->coefficient = coefficient;
		this->offset = offset;
		this->power = power;
	}
	functionTerm(std::string type, double coefficient, double offset, double power) {
		// for a quadratic, it is in the form of coefficient * (t^power) + offset
		this->type = type; //should be "quad"
		this->amplitude = 1;
		this->coefficient = coefficient;
		this->offset = offset;
		this->power = power;
	}

	double evaluate(double t) {
		if (this->type == "sin") {
			return this->amplitude * (pow(sin(this->coefficient * t * M_PI + this->offset), this->power));
		}
		else if (this->type == "cos") {
			return this->amplitude * (pow(cos(this->coefficient * t * M_PI + this->offset), this->power));
		}
		else if (this->type == "quad") {
			return this->coefficient * (pow(t, this->power)) + this->offset;
		}
		else {
			return 0; //not matching any supported term types
		}
	}
};


template <class S = double>
class functionGenerator : public generator<S> {
	public:
		double num_samples;
		double time_step;
		double offset;
		std::vector<functionTerm> arr;
		double current_time;
		functionGenerator(size_t num_samples, size_t time_step, size_t offset, std::vector<functionTerm> arr) : generator<S>(1) {
			this->num_samples = num_samples;
			this->time_step = time_step;
			this->offset = offset;
			this->arr = arr;
			this->current_time = offset;
		}

		int generate() {
			auto newData = this->output.getData();
			this->bufferLock.lock();
			for (int c = 0; c < newData.size(); ++c) {
				double sum = 0;
				for (auto term : this->arr) {
					sum += term.evaluate(this->current_time);
				}
				this->current_time += this->time_step;
				newData[c] = sum;
			}
			this->bufferLock.unlock();
			return newData.size() * sizeof(S);
		}

		double generateSingle(double t) {
			double sum = 0;
			for (auto term : this->arr) {
				sum += term.evaluate(t);
			}
			return sum;
		}
	private:
};


#endif