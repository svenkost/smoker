/*
 * TemperatureSensors.h
 *
 *  Created on: 5 feb. 2014
 *      Author: svenvandermeij
 */

#ifndef TEMPERATURESENSORS_H_
#define TEMPERATURESENSORS_H_

#include "Arduino.h"
#include <stdlib.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMPERATURE_PRECISION 9

class TemperatureSensors {
protected:
	OneWire* owTempSensor;
	DallasTemperature* tempSensors;
	DeviceAddress *ambientThermometer;
	uint8_t barrel_pin, meat_pin;
	float barrelTemp;
	float meatTemp;

	float getNTCTemp(int analogValue, float rBalance, float R25, float tempCoefficient);
	float getVoltage(int analogValue);
	float getTemperature(DeviceAddress *addr);
public:
	TemperatureSensors(uint8_t owpin, uint8_t barrelPin, uint8_t meatPin);
	float getBarrelTemperature(void);
	float getMeatTemperature(void);
	float getAmbientTemperature(void);
};

#endif /* TEMPERATURESENSORS_H_ */
