#ifndef RGBLed_h_
#define RGBLed_h_

#include <inttypes.h>

#include "Arduino.h"

typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} Color;

extern const Color COLOR_GREEN;
extern const Color COLOR_RED;
extern const Color COLOR_BLUE;
extern const Color COLOR_BLACK;
extern const Color COLOR_WHITE;
extern const Color COLOR_YELLOW;
extern const Color COLOR_CYAN;
extern const Color COLOR_MAGENTA;


class RGBLed {
private:
	uint8_t pinR;
	uint8_t pinG;
	uint8_t pinB;
	
public:
	RGBLed(uint8_t pin_R, uint8_t pin_G, uint8_t pin_B);
	void off(void);
	void changeColor(const Color* col);
	void changeColor(uint8_t r, uint8_t g, uint8_t b);

private:
	void writeValue (uint8_t pin, int value);
};

#endif
