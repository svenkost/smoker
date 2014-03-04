/*
 * SmokerRegulator.h
 *
 *  Created on: 1 mrt. 2014
 *      Author: svenvandermeij
 */

#ifndef SMOKERREGULATOR_H_
#define SMOKERREGULATOR_H_

#include <PID_v1.h>
#include <GasValve.h>

class SmokerRegulator {
protected:
	PID *myPID;
	bool measured_in_meat;
	double *currentTemp, *targetTemp, output;
	GasValve *gasValve;
	SmokeValve *smokeValve;
public:
	SmokerRegulator(double *curTemp, double *targTemp, bool measure_in_meat, GasValve *gasvalve, SmokeValve *smokevalve, int interval);
	void start(void);
	void stop(void);
	void regulate(void);

};

#endif /* SMOKERREGULATOR_H_ */
