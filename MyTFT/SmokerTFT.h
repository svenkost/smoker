/*
 * SmokerTFT.h
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */

#ifndef SMOKERTFT_H_
#define SMOKERTFT_H_


class SmokerTFT {
protected:
	float last_ambient, last_barrel, last_barrelamb, last_meat, last_target;
	int last_gasvalve, last_smokevalve, last_smoking_started;
	long last_secondsRemaining;
	bool last_is_meat_selected;
	bool init_temps, init_valves;
	void init(void);

public:

	void displayTime(tmElements_t *time);
	void displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected);
	void displayValveStatus(int smokeValve, int gasValve);
	void displayTimeRemaining(unsigned long secondsRemaining, int smokingStarted);

};


#endif /* SMOKERTFT_H_ */
