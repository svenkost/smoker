/*
 * Logger.h
 *
 *  Created on: 5 feb. 2014
 *      Author: svenvandermeij
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "Arduino.h"
#include <stdlib.h>

#include <mmc.h>
#include <tinyFAT.h>
#include <DS1307RTC.h>
#include <Time.h>

class Logger {
protected:
	char* filename;
	char* getCurrentTimeStamp(void);
	char* addCurrentTimeStamp(char* text);
	char* verboseError(byte err);

public:
	Logger(uint8_t pinSD_CS);
	~Logger();
	char* createUniqueFileName(void);

	void writeLoggingInfo(double ambTemp, double meatTemp, double barrelTemp,
			double targetTemp, int targetTempLocation, double used_temp,
			double tempdiff_now, double tempdiff_last, long gasValveStep,
			long smokeValveStep);
	void writeln(char* text);
};

#endif /* LOGGER_H_ */
