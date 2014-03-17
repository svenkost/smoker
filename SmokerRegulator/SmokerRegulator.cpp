/*
 * SmokerRegulator.cpp
 *
 *  Created on: 1 mrt. 2014
 *      Author: svenvandermeij
 */
#include <SmokerRegulator.h>
const double MIN = 0;
const double MAX = 255;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;
double old_output =0;

SmokerRegulator::SmokerRegulator(double *curTemp, double *targTemp, bool measure_in_meat, GasValve *gasvalve, SmokeValve *smokevalve, int interval) {
	Serial.println("Initialize regulator");
	measured_in_meat = measure_in_meat;
	currentTemp = curTemp;
	targetTemp = targTemp;
	smokeValve = smokevalve;
	gasValve = gasvalve;
	output=0;
	if (measured_in_meat) {
		myPID = new PID(currentTemp, &output, targetTemp, consKp, consKi, consKd, DIRECT);
	} else {
		myPID = new PID(currentTemp, &output, targetTemp, aggKp, aggKi, aggKd, DIRECT);
	}
	myPID->SetSampleTime(interval);
	myPID->SetOutputLimits(MIN, MAX);
}

void SmokerRegulator::start() {
	Serial.println("Starting regulator");
	myPID->SetMode(AUTOMATIC);
}

void SmokerRegulator::stop() {
	Serial.println("Stopping regulator");
	myPID->SetMode(MANUAL);
}

void SmokerRegulator::regulate() {
	Serial.print("Regulating.");
	double diff = *targetTemp - *currentTemp;
	Serial.print("Regulating. tempdiff = ");
	Serial.print(diff);
	Serial.print("; cur_temp = ");
	Serial.print(*currentTemp);
	Serial.print("; Target = ");
	Serial.println(*targetTemp);

	if (!measured_in_meat) {
		if (diff < 10) { // now use conservative settings
			Serial.println("Regulating: changing parameters to conservative");
			myPID->SetTunings(consKp, consKi, consKd);
		} else { // use aggressive settings
			Serial.println("Regulating: changing parameters to aggressive");
			myPID->SetTunings(aggKp, aggKi, aggKd);
		}
	}
	Serial.println("Regulating: Before compute");
	old_output = output;
	if (myPID->Compute()) {
		// Now determine the gas/smoke valve settings and change them with 'output' from the PID routine as
		// main parameter.
		Serial.print("Regulating: After compute. Now checking output:");
		Serial.println(output);
		double output_diff = output - old_output;
		if (output >= MAX) { // max, dus go up
			gasValve->fullyOpen();
			smokeValve->fullyClose();
		} else if (output <= MIN) {
			gasValve->fullyClose();
			smokeValve->fullyOpen();
		} else if (output_diff > 0) { // go up
			// Now make it so the temp will rise. How much is the number in output_diff

		} else {// go down
			// Now make it so the temp will drop. How much is the number in output_diff

		}
	}
}


