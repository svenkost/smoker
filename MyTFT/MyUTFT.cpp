/*
 * MyUTFT.cpp
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */
#include <MyUTFT.h>

MyUTFT::MyUTFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_MOSI, uint8_t TFT_CLK, uint8_t TFT_RST, uint8_t TFT_MISO) {
	tft = new UTFT(TFT01_22SP, TFT_MOSI, TFT_CLK, TFT_CS, TFT_RST, TFT_DC);
	init();
}

void MyUTFT::init() {
	tft->InitLCD();
	tft->setBackColor(255,255,255);
	tft->fillScr(255,255,255);

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
	last_tempdif = -1;
}

UTFT* MyUTFT::getTFT() {
	return tft;
}

void MyUTFT::displayTime(tmElements_t *time) {
//	tft->setColor(255, 255, 255);
//	tft->fillRect(80, 2, 240, 9);
	tft->setColor(0, 0, 0);
	tft->drawRect(0, 0, 319, 11);
	tft->setFont(TinyFont);
	String timestr = String(time->Day)+
			String("-")+
			String(time->Month)+
			String("-")+
			String(tmYearToCalendar(time->Year))+
			String(time->Hour<10?"   0":"   ")+
			String(time->Hour)+
			String(time->Minute<10?":0":":")+
			String(time->Minute)+
			String(time->Second<10?":0":":")+
			String(time->Second);
	tft->print(timestr, CENTER, 2);
}

void MyUTFT::displayTemps(float ambient, float meat, float barrelamb, float barrel, float target, bool isMeatSelected) {

	if (ambient != last_ambient) {
		last_ambient=ambient;
		tft->setColor(0,120,0);//green
		if (!init_temps) {
			tft->setFont(TinyFont);
			tft->drawRect(0,210,80,240);
			tft->print("Omgeving", 3, 212);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(1,222,79,15+222);
		}
		tft->setColor(0,120,0);//green
		if (ambient > 100 || ambient < 0) {
			tft->setFont(SmallFont);
			tft->printNumF(ambient, 1, 15, 225);
		} else {
			tft->setFont(BigFont);
			tft->printNumF(ambient, 1, 1, 222);
		}
	}
	if (meat != last_meat) {
		last_meat = meat;

		tft->setColor(0xF81F); //magenta
		if (!init_temps) {
			tft->drawRect(80,210,160,240);
			tft->setFont(TinyFont);
			tft->print("Vlees",83,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(81,222,78+81,15+222);
		}
		tft->setColor(0xF81F); //magenta
		if (meat > 100 || meat < 0) {
			tft->setFont(SmallFont);
			tft->printNumF(meat,1,95,225);
		} else {
			tft->setFont(BigFont);
			tft->printNumF(meat,1,81,222);
		}
	}
	if (barrelamb != last_barrelamb) {
		last_barrelamb = barrelamb;
		tft->setColor(205, 92, 92); //indian red
		if (!init_temps) {
			tft->setColor(205, 92, 92); //indian red
			tft->drawRect(160,210,240,239);
			tft->setFont(TinyFont);
			tft->print("Vat pen",163,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(161,222,238,237);
		}
		tft->setColor(205, 92, 92); //indian red
		if (barrelamb > 100 || barrelamb < 0) {
			tft->setFont(SmallFont);
			tft->printNumF(barrelamb,1,175,225);
		} else {
			tft->setFont(BigFont);
			tft->printNumF(barrelamb,1,161,222);
		}

	}
	if (barrel != last_barrel) {
		last_barrel = barrel;
		tft->setColor(153,50,204);
		if (!init_temps) {
			tft->drawRect(240,210,319,239);
			tft->setFont(TinyFont);
			tft->print("Vat",243,212);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(241,222,78+241,15+222);
		}
		tft->setColor(153,50,204);
		if (barrel > 100 || barrel < 0) {
			tft->setFont(SmallFont);
			tft->printNumF(barrel, 1, 255,225);
		} else {
			tft->setFont(BigFont);
			tft->printNumF(barrel, 1, 241,222);
		}
	}
	if (target != last_target) {
		last_target = target;
		tft->setColor(112, 75, 4);
		if (!init_temps) {
			tft->drawRect(106,180,110+106,30+180);
			tft->setFont(TinyFont);
			tft->print("Gekozen temp",110,182);
		} else {
			tft->setColor(255,255,255);
			tft->fillRect(110,192,104+110,15+192);
		}
		tft->setColor(112, 75, 4);
		tft->setFont(BigFont);
		tft->printNumF(target,1,110,192);
	}
	if (last_is_meat_selected != isMeatSelected) {
		last_is_meat_selected = isMeatSelected;
		if (isMeatSelected) {
			tft->setColor(0xF81F); //magenta
			tft->drawRect(81, 211, 78+81, 28+211);

			tft->setColor(255,255,255);
			tft->drawRect(241, 211, 78+241, 28+211);
		} else {
			tft->setColor(255,255,255);
			tft->drawRect(81, 211, 78+81, 28+211);
			tft->setColor(153,50,204);
			tft->drawRect(241, 211, 78+241, 28+211);
		}
	}
	init_temps=true;
}

void MyUTFT::displayValveStatus(int smokeValve, int gasValve) {
	if (smokeValve != last_smokevalve) {
		tft->setColor(255, 0, 127); //pink

		last_smokevalve = smokeValve;
		if (!init_valves) {
			tft->drawRect(0,180,106,210);
			tft->setFont(TinyFont);
			tft->print("Rook klep",2,182);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(1,192,103,15+192);
		}
		tft->setColor(255, 0, 127); //pink
		tft->setFont(BigFont);
		tft->printNumI(smokeValve,3,192);
	}

	if (gasValve != last_gasvalve) {
		tft->setColor(0, 123, 123); //dark cyan
		last_gasvalve = gasValve;
		if (!init_valves) {
			tft->setColor(0, 123, 123); //dark cyan
			tft->drawRect(216,180,319,210);
			tft->setFont(TinyFont);
			tft->print("Gas klep",220,182);
		} else {
			tft->setColor(255, 255, 255);
			tft->fillRect(220,192,318,15+192);
		}
		tft->setColor(0, 123, 123); //dark cyan
		tft->setFont(BigFont);
		tft->printNumI(gasValve,220,192);
	}
	init_valves = true;

}

void MyUTFT::displayTimeRemainingOrTargetTemp(unsigned long secondsRemaining, double temp_diff,
		int smokingStarted, bool displayTime) {

	if (displayTime) {
		if (last_secondsRemaining != secondsRemaining) {
			last_secondsRemaining = secondsRemaining;
			last_tempdif = -1;
			int hours = secondsRemaining/3600;
			int minutes = (secondsRemaining%3600)/60;
			int seconds = (secondsRemaining%3600)%60;
			tft->setColor(255,255,255);
			tft->fillRect(40,124,319,178);

			tft->setColor(255,0,0);
			tft->drawRect(0,120,319,180);

			tft->setFont(SevenSegNumFontPlus);
			String timestr = String(hours)+
					String(minutes<10?":0":":")+
					String(minutes)+
					String(seconds<10?":0":":")+
					String(seconds);
			tft->print(timestr, 40, 124);
		}
	} else { //Display target temp
		if (last_tempdif != temp_diff) {
			last_secondsRemaining = -1;
			last_tempdif = temp_diff;
			tft->setColor(255,255,255);
			tft->fillRect(40,124,319,178);
			tft->setColor(255,0,0);
			tft->drawRect(0,120,319,180);
			tft->setFont(SixteenSegment);
			tft->printNumF(temp_diff, 1, 80, 124);
		}
	}

	if (last_smoking_started != smokingStarted) {
		last_smoking_started = smokingStarted;
		if (smokingStarted==1) {
			tft->setColor(0,255,0);
			tft->fillCircle(15, 150, 10);
		} else {
			tft->setColor(255,0,0);
			tft->fillCircle(15, 150, 10);
		}
	}

}
