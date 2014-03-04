/*
 * SmokerTFT.h
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */

#ifndef SMOKERTFT_H_
#define SMOKERTFT_H_

#include "Arduino.h"
#include <stdlib.h>
#include <Time.h>


class SmokerTFT {
protected:
	float last_ambient, last_barrel, last_barrelamb, last_meat, last_target;
	int last_gasvalve, last_smokevalve, last_smoking_started;
	long last_secondsRemaining;
	bool last_is_meat_selected;
	bool init_temps, init_valves;
	virtual void init(void)=0;

public:

	virtual void displayTime(tmElements_t *time)=0;
	virtual void displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected)=0;
	virtual void displayValveStatus(int smokeValve, int gasValve)=0;
	virtual void displayTimeRemaining(unsigned long secondsRemaining, int smokingStarted)=0;

};


#endif /* SMOKERTFT_H_ */
