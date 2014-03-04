/*
 * MyUTFT.h
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */

#ifndef MYUTFT_H_
#define MYUTFT_H_
#include "Arduino.h"
#include <stdlib.h>
#include <Time.h>
#include <memorysaver.h>
#include <UTFT.h>

extern uint8_t SmallFont[];
extern uint8_t TinyFont[];
extern uint8_t SevenSegNumFontPlus[];
extern uint8_t BigFont[];

class MyUTFT {
protected:
	UTFT *tft;
	float last_ambient, last_barrel, last_barrelamb, last_meat, last_target;
	int last_gasvalve, last_smokevalve, last_smoking_started;
	long last_secondsRemaining;
	bool last_is_meat_selected;
	bool init_temps, init_valves;
	void init(void);

public:
	MyUTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO);
	UTFT* getTFT(void);
	void displayTime(tmElements_t *time);
	void displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected);
	void displayValveStatus(int smokeValve, int gasValve);
	void displayTimeRemaining(unsigned long secondsRemaining, int smokingStarted);

};

#endif /* MYUTFT_H_ */
