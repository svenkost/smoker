/*
 * GasValve.h
 *
 *  Created on: 3 feb. 2014
 *      Author: svenvandermeij
 */

#ifndef GASVALVE_H_
#define GASVALVE_H_

#include "Arduino.h"

#include <stdlib.h>
#include "Encoder.h"
#include <Servo.h>

class MyStepper {
protected:
	uint8_t pinA, pinB, pinC, pinD;
	int delayTime;
public:
	MyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);
	void step(int count);
};

class Valve {
protected:
	long current_position;
public:
	virtual void fullyClose(void)=0;
	virtual void fullyOpen(void)=0;
	virtual bool close(int steps)=0;
	virtual bool open(int steps)=0;
	virtual bool isFullyOpen(void)=0;
	virtual bool isFullyClosed(void)=0;
	long getCurrentPosition(void);
};

class GasValve : public Valve {
protected:
	Encoder* gasValveCheck;
	MyStepper* gasValveStepper;
	bool flame_min; // Indicator that the motor is at the beginning
	bool flame_max; // Indicator that the motor is at the end


	bool stepWithREFeedback(int steps);
public:
	GasValve(Encoder *re, uint8_t pinA, uint8_t pinB, uint8_t pinC, uint8_t pinD);
	void fullyClose(void);
	void fullyOpen(void);
	bool isFullyOpen(void);
	bool isFullyClosed(void);
	bool close(int steps);
	bool open(int steps);
};

class SmokeValve: public Valve {
protected:
	int minpos, maxpos;
	long delaytime;
	Servo *smokeValveServo;
	void init(uint8_t pin, int min, int max);

public:
	SmokeValve(uint8_t pin, int min, int max);
	SmokeValve(uint8_t pin);
	void fullyClose(void);
	void fullyOpen(void);
	bool isFullyOpen(void);
	bool isFullyClosed(void);
	void absolutePercentage(int percent);
	bool close(int steps);
	bool open(int steps);

};

#endif /* GASVALVE_H_ */
