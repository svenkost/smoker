/*
 * TFT.h
 *
 *  Created on: 7 feb. 2014
 *      Author: svenvandermeij
 */

#ifndef MYTFT_H_
#define MYTFT_H_

#include "Arduino.h"
#include <stdlib.h>
#include <Time.h>

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SmokerTFT.h>

class MyTFT : public SmokerTFT {
protected:
	Adafruit_ILI9341 *tft;
public:
	MyTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO);
	MyTFT(Adafruit_ILI9341* t);
	Adafruit_ILI9341* getTFT(void);
};

#endif /* MYTFT_H_ */
