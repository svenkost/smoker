/*
 * SmokerRegulator.cpp
 *
 *  Created on: 1 mrt. 2014
 *      Author: svenvandermeij
 */
#include <SmokerRegulator.h>

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

SmokerRegulator::SmokerRegulator(double *curTemp, double *targTemp, bool measure_in_meat, GasValve *gasvalve, SmokeValve *smokevalve, int interval) {
	measured_in_meat = measure_in_meat;
	currentTemp = curTemp;
	targetTemp = targTemp;
	smokeValve = smokevalve;
	gasValve = gasvalve;
	if (measured_in_meat) {
		myPID = new PID(currentTemp, &output, targetTemp, consKp, consKi, consKd, DIRECT);
	} else {
		myPID = new PID(currentTemp, &output, targetTemp, aggKp, aggKi, aggKd, DIRECT);
	}
	myPID->SetSampleTime(interval);
}

void SmokerRegulator::start() {
	myPID->SetMode(AUTOMATIC);
}

void SmokerRegulator::stop() {
	myPID->SetMode(MANUAL);
}

void SmokerRegulator::regulate() {
	double diff = *targetTemp - *currentTemp;

	if (!measured_in_meat) {
		if (diff < 10) { // now use conservative settings
			myPID->SetTunings(consKp, consKi, consKd);
		} else { // use aggressive settings
			myPID->SetTunings(aggKp, aggKi, aggKd);
		}
	}
	if (myPID->Compute()) {
		// Now determine the gas/smoke valve settings and change them with 'output' from the PID routine as
		// main parameter.

	}


}


