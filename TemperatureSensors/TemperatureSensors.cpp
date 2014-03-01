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

const int MAX_SAMPLES_NTC=10;

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
	float temps[MAX_SAMPLES_NTC], min_temp, max_temp, avg=0.0f;
	int min_index, max_index;

	for (int i=0;i<MAX_SAMPLES_NTC;i++) {
		temps[i] = getNTCTemp(analogRead(pin), (float)RBALANCE, RNTCNOMINAL, 4783);
		if (i==0) {
			min_temp = temps[0];
			max_temp = temps[0];
			min_index = 0;
			max_index = 0;
		} else {
			if (temps[i]<min_temp) {
				min_temp = temps[i];
				min_index = i;
			};
			if (temps[i]>max_temp) {
				max_temp = temps[i];
				max_index = i;
			}
		}
	}

	for (int i=0;i<MAX_SAMPLES_NTC;i++) {
		if (i != min_index && i != max_index) {
			avg += temps[i];
		}
	}
	avg /= MAX_SAMPLES_NTC-2;
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
