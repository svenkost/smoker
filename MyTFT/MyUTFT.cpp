/*
 * MyUTFT.cpp
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */
#include <MyUTFT.h>

MyUTFT::MyUTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO) {
	tft = new UTFT(TFT01_22SP, TFT_MOSI, TFT_CLK, TFT_CS, TFT_RST, TFT_DC);
}

void MyUTFT::init() {
	tft->InitLCD();
	tft->setBackColor(255,255,255);
	tft->clrScr();

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

UTFT* MyUTFT::getTFT() {
	return tft;
}

void MyUTFT::displayTime(tmElements_t *time) {
	tft->setColor(0, 0, 0);
	tft->drawRect(0, 0, 320, 11);
	tft->setFont(TinyFont);
	tft->print(time->Day+"-"+time->Month+"-"+tmYearToCalendar(time->Year)+"   "+time->Hour+":"+time->Minute+":"+time->Second, CENTER, 2);
}

void MyUTFT::displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected) {

	if (ambient != last_ambient) {
		last_ambient=ambient;
		tft->setFont(TinyFont);
		tft->setColor(0,120,0);//green
		if (!init_temps) {
			tft->drawRect(0,210,80,30);
			tft->print("Omgevingtemp", 3, 212);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(1,222,78,15);
		}
		tft->setFont(BigFont);

		tft->printNumF(ambient, 2, 3, 222);
	}
	if (meat != last_meat) {
		last_meat = meat;

		tft->setColor(0xF81F); //magenta
		if (!init_temps) {
			tft->drawRect(80,210,80,30);
			tft->setFont(TinyFont);
			tft->print("Vlees temp",83,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(81,222,78,15);
		}
		tft->setFont(BigFont);
		tft->printNumF(meat, 2,83,222);
	}
	if (barrelamb != last_barrelamb) {
		last_barrelamb = barrelamb;
		tft->setColor(205, 92, 92); //indian red
		if (!init_temps) {
			tft->drawRect(160,210,80,30);
			tft->setFont(TinyFont);
			tft->print("Vat pen temp",163,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(161,222,78,15);
		}
		tft->setFont(BigFont);
		tft->printNumF(barrelamb,2,163,222);
	}
	if (barrel != last_barrel) {
		last_barrel = barrel;
		tft->setColor(153,50,204);
		if (!init_temps) {
			tft->drawRect(240,210,80,30);
			tft->setFont(TinyFont);
			tft->print("Vat temp",243,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(241,222,78,15);
		}
		tft->setFont(BigFont);
		tft->printNumF(barrel, 2, 243,222);
	}
	if (target != last_target) {
		last_target = target;
		tft->setColor(112, 75, 4);
		if (!init_temps) {
			tft->drawRect(106,180,110,30);
			tft->setFont(TinyFont);
			tft->print("Gekozen temp",110,182);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(110,192,104,15);
		}
		tft->setFont(BigFont);
		tft->printNumF(target,2,110,192);
	}
	if (last_is_meat_selected != isMeatSelected) {
		last_is_meat_selected = isMeatSelected;
		if (isMeatSelected) {
			tft->setColor(0xF81F); //magenta
			tft->drawRect(81, 211, 78, 28);

			tft->setColor(255,255,255);
			tft->drawRect(241, 211, 78, 28);
		} else {
			tft->setColor(255,255,255);
			tft->drawRect(81, 211, 78, 28);
			tft->setColor(153,50,204);
			tft->drawRect(241, 211, 78, 28);
		}
	}
	init_temps=true;

}

void MyUTFT::displayValveStatus(int smokeValve, int gasValve) {
	if (smokeValve != last_smokevalve) {
		tft->setColor(255, 0, 127); //pink

		last_smokevalve = smokeValve;
		if (!init_valves) {
			tft->drawRect(0,180,106,30);
			tft->setFont(TinyFont);
			tft->print("Rook klep",2,182);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(1,192,102,15);
		}
		tft->setFont(BigFont);
		tft->printNumI(smokeValve,3,192);
	}

	if (gasValve != last_gasvalve) {
		tft->setColor(0, 123, 123); //dark cyan
		last_gasvalve = gasValve;
		if (!init_valves) {
			tft->drawRect(216,180,104,30);
			tft->setFont(TinyFont);
			tft->print("Gas klep",220,182);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(220,192,99,15);
		}
		tft->setFont(BigFont);
		tft->printNumI(gasValve,220,192);
	}
	init_valves = true;

}

void MyUTFT::displayTimeRemaining(unsigned long secondsRemaining, int smokingStarted) {
	if (last_secondsRemaining != secondsRemaining) {
		last_secondsRemaining = secondsRemaining;
		int hours = secondsRemaining/3600;
		int minutes = (secondsRemaining%3600)/60;
		int seconds = (secondsRemaining%3600)%60;
		tft->setColor(255,0,0);
		tft->drawRect(0,130,320,50);

		tft->setFont(SevenSegNumFontPlus);
		tft->setColor(255,255,255);
		tft->fillRect(60,134,255,42);
		tft->print(hours+(minutes<10?":0":":")+minutes+(seconds<10?":0":":")+seconds, 60, 134);
	}
	if (last_smoking_started != smokingStarted) {
		last_smoking_started = smokingStarted;
		if (smokingStarted==1) {
			tft->setColor(0,255,0);
			tft->fillCircle(15, 155, 10);
		} else {
			tft->setColor(255,0,0);
			tft->fillCircle(15, 155, 10);
		}
	}

}
