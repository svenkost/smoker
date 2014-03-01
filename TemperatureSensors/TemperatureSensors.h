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
#include <AD595.h>

#define TEMPERATURE_PRECISION 9

class TemperatureSensors {
protected:
	OneWire* owTempSensor;
	DallasTemperature* tempSensors;
	DeviceAddress *ambientThermometer;

	AD595 *barrelThermocouple;

	uint8_t barrel_pin, meat_pin;
	float barrelTemp;
	float meatTemp;

	float getNTCTemp(int analogValue, float rBalance, float R25, float tempCoefficient);
	float getNTCAvgTemp(uint8_t pin);
	float getVoltage(int analogValue);
	float getTemperature(DeviceAddress *addr);
public:
	TemperatureSensors(uint8_t owpin, uint8_t ovenAmbientNTCPin, uint8_t meatNTCPin, uint8_t barrelTCPin);
	float getBarrelTemperature(void);
	float getMeatTemperature(void);
	float getAmbientTemperature(void);
	float getOvenAmbientTemperature(void);
};

#endif /* TEMPERATURESENSORS_H_ */
