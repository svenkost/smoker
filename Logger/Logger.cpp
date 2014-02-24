/*
 * Logger.cpp
 *
 *  Created on: 5 feb. 2014
 *      Author: svenvandermeij
 */

#include "Logger.h"

Logger::Logger(uint8_t pinSD_CS) {
	filename = (char*) malloc(13 * sizeof(char));
	/* Initialize the SD Card slot */
	file.setSSpin(pinSD_CS);

	byte res = file.initFAT(); // Try to connect to the SD card.
	if (res != NO_ERROR) {
		Serial.print("***** ERROR: ");
		Serial.println(verboseError(res));
		while (true) {
		};
	}
}

Logger::~Logger() {
	free(filename);
}
;

char *Logger::createUniqueFileName() {
	tmElements_t tm;
	if (RTC.read(tm)) {
		int counter = 0;
		do {
			sprintf(filename, "%02d%02d%02d%02d.%d", tm.Day, tm.Month, tm.Hour,
					tm.Minute, counter++);
		} while (file.exists(filename));
	} else {
		int counter = 0;
		do {
			sprintf(filename, "smoker%d.log", counter++);
		} while (file.exists(filename));
	};
	file.create(filename);
	return filename;
}
;

char *Logger::verboseError(byte err) {
	switch (err) {
	case ERROR_MBR_READ_ERROR:
		return "Error reading MBR";
		break;
	case ERROR_MBR_SIGNATURE:
		return "MBR Signature error";
		break;
	case ERROR_MBR_INVALID_FS:
		return "Unsupported filesystem";
		break;
	case ERROR_BOOTSEC_READ_ERROR:
		return "Error reading Boot Sector";
		break;
	case ERROR_BOOTSEC_SIGNATURE:
		return "Boot Sector Signature error";
		break;
	default:
		return "Unknown error";
		break;
	}
}
/*
ambient: 123.1, barrel: 123.1, meat: 123.1, target: 123.5, location: BARREL, current_temp_diff: 123.1, last_temp_diff: 123.1, gasValvePosition: 10000, smokeValvePosition: 999
*/
void Logger::writeLoggingInfo(double ambTemp, double meatTemp,
		double barrelTemp, double ovenAmbTemp, double targetTemp, int targetTempLocation,
		double used_temp, double tempdiff_now, double tempdiff_last,
		long gasValveStep, long smokeValveStep)
{
	char* text = (char*) malloc(200 * sizeof(char));

	sprintf(text,
			"ambient: %3.1f, barrel: %3.1f, meat: %3.1f, ovenambient: %3.1f, target: %3.1f, location: %s, current_temp_diff: %3.1f, last_temp_diff: %3.1f, gasValvePosition: %l, smokeValvePosition: %l",
			ambTemp, barrelTemp, meatTemp, ovenAmbTemp, targetTemp, targetTempLocation?"MEAT":"BARREL", targetTemp, tempdiff_now, tempdiff_last, gasValveStep, smokeValveStep);

	writeln(text);
	free(text);
}

void Logger::writeln(char* text) {
	char* tekst;
	file.openFile(filename, FILEMODE_TEXT_WRITE);

	tekst = addCurrentTimeStamp(text);
	file.writeLn(tekst);
	file.closeFile();
	if (Serial) {
		Serial.println(tekst);
	}
	free(tekst);
}

char* Logger::addCurrentTimeStamp(char* text) {
	char* datetimestamp = (char*) malloc(22 * sizeof(char) + sizeof(text));

	tmElements_t tm;
	if (RTC.read(tm)) {
		sprintf(datetimestamp, "%02d-%02d-%04d %02d:%02d:02d %s", tm.Day,
				tm.Month, tmYearToCalendar(tm.Year), tm.Hour, tm.Minute,
				tm.Second, text);
	} else {
		sprintf(datetimestamp, "%s", text);
	}

	return datetimestamp;
}

char* Logger::getCurrentTimeStamp(void) {
	char* datetimestamp = (char*) malloc(22 * sizeof(char));

	tmElements_t tm;
	if (RTC.read(tm)) {
		sprintf(datetimestamp, "%02d-%02d-%04d %02d:%02d:02d ", tm.Day,
				tm.Month, tmYearToCalendar(tm.Year), tm.Hour, tm.Minute,
				tm.Second);
	} else {
		*datetimestamp = '\0';
	}

	return datetimestamp;
}
