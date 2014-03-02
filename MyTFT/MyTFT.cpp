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
	init_temps=false;
	init_valves=false;
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
		tft->setTextColor(col);
		if (!init_temps) {
			tft->drawRect(0,210,80,30,col);
			tft->setTextSize(1);
			tft->setCursor(3,212);
			tft->println("Omgevingtemp");
		} else {
			tft->fillRect(1,222,78,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(3,222);
		tft->print(ambient);
	}
	if (meat != last_meat) {
		last_meat = meat;

		tft->setTextColor(ILI9341_MAGENTA);
		if (!init_temps) {
			tft->drawRect(80,210,80,30, ILI9341_MAGENTA);
			tft->setTextSize(1);
			tft->setCursor(83,212);
			tft->println("Vlees temp");
		} else {
			tft->fillRect(81,222,78,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(83,222);
		tft->print(meat);
	}
	if (barrelamb != last_barrelamb) {
		last_barrelamb = barrelamb;
		uint16_t col = tft->color565(205, 92, 92); //indian red
		tft->setTextColor(col);
		if (!init_temps) {
			tft->drawRect(160,210,80,30, col);
			tft->setTextSize(1);
			tft->setCursor(163,212);
			tft->println("Vat pen temp");
		} else {
			tft->fillRect(161,222,78,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(163,222);
		tft->print(barrelamb);
	}
	if (barrel != last_barrel) {
		last_barrel = barrel;
		uint16_t col = tft->color565(153,50,204);
		tft->setTextColor(col);
		if (!init_temps) {
			tft->drawRect(240,210,80,30, col);
			tft->setTextSize(1);
			tft->setCursor(243,212);
			tft->println("Vat temp");
		} else {
			tft->fillRect(241,222,78,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(243,222);
		tft->print(barrel);
	}
	if (target != last_target) {
		last_target = target;
		uint16_t col = tft->color565(112, 75, 4);
		tft->setTextColor(col);
		if (!init_temps) {
			tft->drawRect(106,180,110,30, col);
			tft->setTextSize(1);
			tft->setCursor(110,182);
			tft->println("Gekozen temp");
		} else {
			tft->fillRect(110,192,104,15,ILI9341_WHITE);
		}
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
			uint16_t col = tft->color565(153,50,204);
			tft->drawRect(81, 211, 78, 28, ILI9341_WHITE);
			tft->drawRect(241, 211, 78, 28, col);
		}
	}
	init_temps=true;
}

void MyTFT::displayTimeRemaining(unsigned long secondsRemaining, int smokingStarted) {
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
		tft->setTextColor(col);
		if (!init_valves) {
			tft->drawRect(0,180,106,30, col);
			tft->setTextSize(1);
			tft->setCursor(2,182);
			tft->println("Rook klep");
		} else {
			tft->fillRect(1,192,102,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(3,192);
		tft->print(smokeValve);
	}

	if (gasValve != last_gasvalve) {
		uint16_t col = tft->color565(0, 123, 123); //dark cyan
		last_gasvalve = gasValve;
		tft->setTextColor(col);
		if (!init_valves) {
			tft->drawRect(216,180,104,30, col);
			tft->setTextSize(1);
			tft->setCursor(220,182);
			tft->println("Gas klep");
		} else {
			tft->fillRect(220,192,99,15,ILI9341_WHITE);
		}
		tft->setTextSize(2);
		tft->setCursor(220,192);
		tft->print(gasValve);
	}
	init_valves = true;
}
