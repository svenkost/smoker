/*
 * TemperatureSensors.cpp
 *
 *  Created on: 5 feb. 2014
 *      Author: svenvandermeij
 */
#include "TemperatureSensors.h"

const int RBALANCE=31750; //30.25 KOhm
const float RNTCNOMINAL=1010000.0f; //1.01MOhm

const float celciusToKelvin=273.15f;
const float boardVoltageWhenOnAC=5.04f;

const int MAX_SAMPLES_NTC=100;

DeviceAddress amb = { 0x28, 0x58, 0x9A, 0x60, 0x5, 0x0, 0x0, 0x3E }; //	28 58 9A 60 05 00 00 3E

TemperatureSensors::TemperatureSensors(uint8_t owpin, uint8_t ovenAmbientNTCPin, uint8_t meatNTCPin, uint8_t barrelTCPin) {
	owTempSensor = new OneWire(owpin);
	tempSensors = new DallasTemperature(owTempSensor);

	// Define all the addresses of the sensors
	ambientThermometer = &amb;

	// initialize the temp sensors;
	tempSensors->begin();

	// Initialize the thermocouple
	barrelThermocouple = new AD595();
	barrelThermocouple->init(barrelTCPin);

	// set the resolution
	tempSensors->setResolution(*ambientThermometer, TEMPERATURE_PRECISION);

	barrel_pin = ovenAmbientNTCPin;
	meat_pin = meatNTCPin;
	barrelTemp=0.0f;
	meatTemp=0.0f;
	stat = new Statistic();
}

float TemperatureSensors::getMeatTemperature(void) {
	meatTemp = getNTCAvgTemp(meat_pin);
	return meatTemp;
}

float TemperatureSensors::getOvenAmbientTemperature(void) {
	barrelTemp = getNTCAvgTemp(barrel_pin);
	return barrelTemp;
}

float TemperatureSensors::getBarrelTemperature(void) {
	return barrelThermocouple->measure(TEMPC);
}

float TemperatureSensors::getAmbientTemperature(void) {
	return getTemperature(ambientThermometer);
}

float TemperatureSensors::getTemperature(DeviceAddress *addr) {
	tempSensors->requestTemperatures();
	return tempSensors->getTempC(*addr);
}

float TemperatureSensors::getNTCAvgTemp(uint8_t pin) {
	float avg;

	stat->clear();
	for (int i=0;i<MAX_SAMPLES_NTC;i++) {
		stat->add(getNTCTemp(analogRead(pin), (float)RBALANCE, RNTCNOMINAL, 4783));
	}
	avg = round(stat->average()*10)/10; // round on one decimal, so the screen is not updated every time

	return avg;
}

float TemperatureSensors::getVoltage(int analogValue) {
	return 5.04f * analogValue / 1024.0f + 0.009f;
}

float TemperatureSensors::getNTCTemp(int analogValue, float rBalance, float R25, float tempCoefficient) {
	if (analogValue == 1023) {
		return 0.0f;
	}

	float u = getVoltage(analogValue);
	float r = u/((boardVoltageWhenOnAC-u)/rBalance);
	float x = log(r/R25)/tempCoefficient+(1.0f/(celciusToKelvin+25.0f));
	float tC = (1.0f/x)-celciusToKelvin;
	return tC;
}
