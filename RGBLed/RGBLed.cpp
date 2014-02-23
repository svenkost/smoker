#include "RGBLed.h"

const Color COLOR_GREEN = {0,255,0};
const Color COLOR_RED = {255,0,0};
const Color COLOR_BLUE = {0,0,255};
const Color COLOR_BLACK = {0,0,0};
const Color COLOR_WHITE = {255,255,255};
const Color COLOR_YELLOW = {255,255,0};
const Color COLOR_CYAN = {0,255,255};
const Color COLOR_MAGENTA = {255,0,255};

RGBLed::RGBLed(uint8_t pin_R, uint8_t pin_G, uint8_t pin_B) {
	pinR = pin_R;
	pinG = pin_G;
	pinB = pin_B;

	pinMode(pinR, OUTPUT);
	pinMode(pinG, OUTPUT);
	pinMode(pinB, OUTPUT);
	off();
}

void RGBLed::off(void) {
	changeColor((const Color*)&COLOR_BLACK);
}

/* 
this method changes the color of a LED 
param led: int[3] contains the pin numbers for R, G, B
param color: int[3] containing {red, green, blue} where all
tree are values between 0 en 255;
*/
void RGBLed::changeColor(const Color* clr) {
  writeValue(pinR, clr->R);
  writeValue(pinG, clr->G);
  writeValue(pinB, clr->B);
}

void RGBLed::changeColor(uint8_t r, uint8_t g, uint8_t b) {
	  writeValue(pinR, r);
	  writeValue(pinG, g);
	  writeValue(pinB, b);
}

/* 
writes a value for a led to a specified pin. It checks wether the pin
is PWM enabled (on a MEGA). If it is, it writes the value as analog to the pin, 
else the input 'value' is treated as a binary: 0 = low and > 0 = high
param pin: the pin to write to
param value: the value to write to the pin 
*/
void RGBLed::writeValue (uint8_t pin, int value) {
  // first determine if the led is PWM or digital
  if ((pin >= 2 && pin <= 13) || (pin>=44 && pin<=46)) { // so it is pwm
    if (value > 255) {
      value = 255;
    }
    if (value < 0) {
      value = 0;
    }
    analogWrite(pin, 255-value);
  } else {
    if (value == 0) {
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }  
  }
}
