/*
 * TFT.cpp
 *
 *  Created on: 7 feb. 2014
 *      Author: svenvandermeij
 */

#include "MyTFT.h"

#include <HardwareSerial.h>

MyTFT::MyTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO) {
	tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
	init();
}

MyTFT::MyTFT(Adafruit_ILI9341* t) {
	tft = t;
	init();
}

void MyTFT::init(void) {
	tft->begin();
	tft->setRotation(3);
	tft->fillScreen(ILI9341_WHITE);

	last_barrel=-1;
	last_meat=-1;
	last_ambient=-1;
	last_target=-1;
	last_is_meat_selected = false;
	last_gasvalve = -1;
	last_smokevalve = -1;
	last_secondsRemaining = -1;
	last_smoking_started =-1;
}

Adafruit_ILI9341* MyTFT::getTFT(void) {
	return tft;
}

void MyTFT::displayTime(tmElements_t *time) {
	tft->drawRect(0, 0, 320, 11, ILI9341_BLACK);
	tft->setCursor(10,2);
	tft->setTextColor(ILI9341_BLACK);
	tft->setTextSize(1);
	tft->print(time->Day);
	tft->print('-');
	tft->print(time->Month);
	tft->print('-');
	tft->print(tmYearToCalendar(time->Year));
	tft->print("   ");
	tft->print(time->Hour);
	tft->print(':');
	tft->print(time->Minute);
	tft->print(':');
	tft->print(time->Second);
}

void MyTFT::displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected) {
	if (ambient != last_ambient) {
		last_ambient=ambient;
		uint16_t col = tft->color565(0, 120, 0); //green
//		tft->drawRect(0,210,106,30, col);
		tft->drawRect(0,210,80,30,col);
		tft->setTextColor(col);
		tft->setTextSize(1);
		tft->setCursor(3,212);
		tft->println("Omgevingtemp");
//		tft->fillRect(1,222,104,15,ILI9341_WHITE);
		tft->fillRect(1,222,78,15,ILI9341_WHITE);
		tft->setTextSize(2);
		tft->setCursor(3,222);
		tft->print(ambient);
	}
	if (meat != last_meat) {
		last_meat = meat;
//		tft->drawRect(106,210,110,30, ILI9341_MAGENTA);
		tft->drawRect(80,210,80,30, ILI9341_MAGENTA);
		tft->setTextColor(ILI9341_MAGENTA);
		tft->setTextSize(1);
//		tft->setCursor(110,212);
		tft->setCursor(83,212);
		tft->println("Vlees temp");
		tft->fillRect(81,222,78,15,ILI9341_WHITE);
		tft->setTextSize(2);
//		tft->setCursor(110,222);
		tft->setCursor(83,222);
		tft->print(meat);
	}
	if (barrelamb != last_barrelamb) {
		last_barrelamb = barrelamb;
		uint16_t col = tft->color565(205, 92, 92); //indian red
		tft->drawRect(160,210,80,30, col);
		tft->setTextColor(ILI9341_RED);
		tft->setTextSize(1);
		tft->setCursor(163,212);
		tft->println("Vat NTC temp");
		tft->fillRect(161,222,78,15,ILI9341_WHITE);
		tft->setTextSize(2);
		tft->setCursor(163,222);
		tft->print(barrelamb);
	}
	if (barrel != last_barrel) {
		last_barrel = barrel;
//		tft->drawRect(216,210,104,30, ILI9341_RED);
		tft->drawRect(240,210,80,30, ILI9341_RED);
		tft->setTextColor(ILI9341_RED);
		tft->setTextSize(1);
//		tft->setCursor(220,212);
		tft->setCursor(243,212);
		tft->println("Vat TC temp");
//		tft->fillRect(220,222,99,15,ILI9341_WHITE);
		tft->fillRect(241,222,78,15,ILI9341_WHITE);
		tft->setTextSize(2);
//		tft->setCursor(220,222);
		tft->setCursor(243,222);
		tft->print(barrel);
	}
	if (target != last_target) {
		last_target = target;
		uint16_t col = tft->color565(223, 150, 4);

		tft->drawRect(106,180,110,30, col);
		tft->setTextColor(col);
		tft->setTextSize(1);
		tft->setCursor(110,182);
		tft->println("Gekozen temp");
		tft->fillRect(110,192,104,15,ILI9341_WHITE);
		tft->setTextSize(2);
		tft->setCursor(110,192);
		tft->print(target);

	}
	if (last_is_meat_selected != isMeatSelected) {
		last_is_meat_selected = isMeatSelected;
		if (isMeatSelected) {
			tft->drawRect(81, 211, 78, 28, ILI9341_MAGENTA);
			tft->drawRect(241, 211, 78, 28, ILI9341_WHITE);
		} else {
			tft->drawRect(81, 211, 78, 28, ILI9341_WHITE);
			tft->drawRect(241, 211, 78, 28, ILI9341_RED);
		}
	}
}

void MyTFT::displayTimeRemaining(long secondsRemaining, int smokingStarted) {
	if (last_secondsRemaining != secondsRemaining) {
		last_secondsRemaining = secondsRemaining;
		int hours = secondsRemaining/3600;
		int minutes = (secondsRemaining%3600)/60;
		int seconds = (secondsRemaining%3600)%60;
		tft->drawRect(0,130,320,50, ILI9341_RED);
		tft->setTextColor(ILI9341_RED);
		tft->setTextSize(5);
		tft->fillRect(60,134,255,42,ILI9341_WHITE);
		tft->setCursor(60,134);
		tft->print(hours);
		tft->print(':');
		if (minutes < 10) {
			tft->print('0');
		}
		tft->print(minutes);
		tft->print(':');
		if (seconds < 10) {
			tft->print('0');
		}
		tft->print(seconds);
	}
	if (last_smoking_started != smokingStarted) {
		last_smoking_started = smokingStarted;
		if (smokingStarted==1) {
			tft->fillCircle(15, 155, 10, ILI9341_GREEN);
		} else {
			tft->fillCircle(15, 155, 10, ILI9341_RED);
		}
	}
}

void MyTFT::displayValveStatus(int smokeValve, int gasValve) {
	if (smokeValve != last_smokevalve) {
		uint16_t col = tft->color565(255, 0, 127); //pink

		last_smokevalve = smokeValve;
		tft->drawRect(0,180,106,30, col);
		tft->setTextColor(col);
		tft->setTextSize(1);
		tft->setCursor(2,182);
		tft->println("Rook klep");
		tft->fillRect(1,192,102,15,ILI9341_WHITE);
		tft->setTextSize(2);
		tft->setCursor(3,192);
		tft->print(smokeValve);

	}

	if (gasValve != last_gasvalve) {
		uint16_t col = tft->color565(0, 123, 123); //dark cyan
		last_gasvalve = gasValve;
		tft->drawRect(216,180,104,30, col);
		tft->setTextColor(col);
		tft->setTextSize(1);
		tft->setCursor(220,182);
		tft->println("Gas klep");
		tft->fillRect(220,192,99,15,ILI9341_WHITE);
		tft->setTextSize(2);
		tft->setCursor(220,192);
		tft->print(gasValve);

	}
}
