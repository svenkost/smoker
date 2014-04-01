/*
 * GasValve.cpp
 *
 *  Created on: 3 feb. 2014
 *      Author: svenvandermeij
 */
#include "GasValve.h"

const int stepSize = 10;

MyStepper::MyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) {
	pinA=pin1;
	pinB=pin2;
	pinC=pin3;
	pinD=pin4;
	delayTime=5;
	pinMode(pinA, OUTPUT);
	pinMode(pinB, OUTPUT);
	pinMode(pinC, OUTPUT);
	pinMode(pinD, OUTPUT);
}


void MyStepper::step(int count) {
	if (count > 0) {
		for (int i=0;i<count;i++) {
			for (int j=0;j<=3;j++) {
				digitalWrite(pinA, j==0?HIGH:LOW);
				digitalWrite(pinB, j==1?HIGH:LOW);
				digitalWrite(pinC, j==2?HIGH:LOW);
				digitalWrite(pinD, j==3?HIGH:LOW);
				delay(delayTime);
			}
		}
	} else {
		for (int i=0;i>count;i--) {
			for (int j=0;j<=3;j++) {
				digitalWrite(pinA, j==3?HIGH:LOW);
				digitalWrite(pinB, j==2?HIGH:LOW);
				digitalWrite(pinC, j==1?HIGH:LOW);
				digitalWrite(pinD, j==0?HIGH:LOW);
				delay(delayTime);
			}
		}
	}
}

/*
 * Constructs the gasvalve class. It uses a rotary encoder to monitor the stepper motor
 */
GasValve::GasValve(Encoder *re, uint8_t pinA, uint8_t pinB, uint8_t pinC, uint8_t pinD) {
	gasValveCheck = re;
	flame_max=false;
	flame_min=false;
	current_position=0;
	// initialize the stepper library
	gasValveStepper = new MyStepper(pinA, pinB, pinC, pinD);
};

/*
 * steps until the number of steps is reached or
 * the rotary encoder doesn't move any more
 * false = not yet at an end; true = end reached
 */
bool GasValve::stepWithREFeedback(int steps) {
	long oldpos, newpos;
	bool end=false;

	// Don't try to open it if it is already fully open, or
	// close it if it is already closed.
	if ((flame_max && steps>0)||(flame_min && steps<0)) {
		return true;
	}

	// Now step, with the rotary encoder check
	for (int i=0;i<abs(steps) && !end;i++){
		oldpos = gasValveCheck->read();
		int step=((steps>0)?1:-1) * stepSize;
		gasValveStepper->step(step);
		newpos = gasValveCheck->read();

		if (oldpos == newpos) { // rotary encoder didn't turn, so it is at it's end
			if (steps>0) { // open
				flame_max=true;
				flame_min=false;
				end=true;
			} else {
				flame_max=false;
				flame_min=true;
				end=true;
			}

		} else { // The rotary encoder did move
			current_position += step;

			if (flame_min && steps>0) {
				flame_min=false;
			}
			if (flame_max && steps<0) {
				flame_max=false;
			}
		}
	}

	return end;
}

/*
 * close the gasvalve
 */
void GasValve::fullyClose(void) {
	// it doesn't really matter how many steps, because it is monitored by
	// a rotary encoder.
	while(!stepWithREFeedback(-2000))
		;
}


/*
 * open the gasvalve
 */
void GasValve::fullyOpen(void) {
	// it doesn't really matter how many steps, because it is monitored by
	// a rotary encoder.
	while (!stepWithREFeedback(2000))
		;
}

/*
 * close the gasvalve a number of steps;
 * true = the valve is at the end
 *
 */
bool GasValve::close(int steps) {
	bool result = stepWithREFeedback(0-steps);

	return result;
}

/*
 * open the gasvalve a number of steps;
 * true = the valve is at the end
 *
 */
bool GasValve::open(int steps) {
	bool result = stepWithREFeedback(steps);

	return result;
}

/*
 * is the gasvalve fully open?
 */
bool GasValve::isFullyOpen(void) {
	return flame_max;
}

/*
 * is the gasvalve fully closed?
 */
bool GasValve::isFullyClosed(void) {
	return flame_min;
}

/*
 * returns the current position for logging purposes
 */
long Valve::getCurrentPosition(void) {
	return current_position;
}

SmokeValve::SmokeValve(uint8_t pin, int min, int max) {
	init(pin, min, max);
};

SmokeValve::SmokeValve(uint8_t pin) {
	init(pin, 0, 180);
};

void SmokeValve::init(uint8_t pin, int min, int max) {
	minpos = min;
	maxpos = max;
	smokeValveServo = new Servo();
	smokeValveServo->attach(pin, minpos, maxpos);
	fullyClose();

};

void SmokeValve::fullyClose(void) {
	current_position = minpos;
	smokeValveServo->write(minpos);
};

void SmokeValve::fullyOpen(void) {
	current_position = maxpos;
	smokeValveServo->write(maxpos);
};

bool SmokeValve::open (int steps) {
	if (current_position+steps > maxpos) {
		current_position = maxpos;
	} else {
		current_position += steps;
	}

	smokeValveServo->write(current_position);

//	while (smokeValveServo.read() != current_position) {
//		delay(100); // wait for the servo to get there
//	}

	return (current_position == maxpos);
};

bool SmokeValve::close (int steps) {
	if (current_position-steps < minpos) {
		current_position = minpos;
	} else {
		current_position -= steps;
	}

	smokeValveServo->write(current_position);

//	while (smokeValveServo.read() != current_position) {
//		delay(100); // wait for the servo to get there
//	}
	return (current_position == minpos);
}

void SmokeValve::absolutePercentage(int percent) {
	int absolute_pos = map(percent, 0, 100, minpos, maxpos);
	current_position = absolute_pos;

	smokeValveServo->write(absolute_pos);
}

bool SmokeValve::isFullyOpen(void) {
	return (current_position == maxpos);
}

bool SmokeValve::isFullyClosed(void) {
	return (current_position == minpos);
}

