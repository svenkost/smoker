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

class MyUTFT : public SmokerTFT {
protected:
	UTFT *tft;
public:
	MyUTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO);
	UTFT* getTFT(void);

};

#endif /* MYUTFT_H_ */
