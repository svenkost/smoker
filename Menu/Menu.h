#ifndef Menu_h_
#define Menu_h_

#include "Arduino.h"
#include <stdlib.h>
#include "MyTFT.h"

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Menu;

class StringOption {
protected:
	const char* option;
	StringOption* next;
	int id;
public:
	StringOption(const char* text);
	const char* getOption(void);
	StringOption* addOption(const char* text);
	void setNext(StringOption* nxt, int id);
	int getID(void);
	void setID(int i);
	const char* getOptionByID(int i);
	StringOption* getNext(void);
};

class Selector {
protected:
	const char* question;
public:
	virtual bool increase()=0;
	virtual bool decrease()=0;
	const char* getQuestion(void);
	virtual char* getValue(void)=0;
};

class IntSelector : public Selector {
private:
	int min;
	int max;
	int *selected;
	int stepping;
	char *temp;
public: 
	IntSelector (const char* question, int* value, int step, int minimum, int maximum);
	~IntSelector(void);
	bool increase(void);
	bool decrease(void);
	char* getValue(void);
};	

class FloatSelector : public Selector {
private:
	float min;
	float max;
	float *selected;
	float stepping;
	char *temp;
public: 
	FloatSelector (const char* question, float* value, float step, float minimum, float maximum);
	~FloatSelector(void);
	bool increase(void);
	bool decrease(void);
	char* getValue(void);
};	

class DoubleSelector : public Selector {
private:
	double min;
	double max;
	double *selected;
	double stepping;
	char *temp;
public:
	DoubleSelector (const char* question, double* value, double step, double minimum, double maximum);
	~DoubleSelector(void);
	bool increase(void);
	bool decrease(void);
	char* getValue(void);
};

class StringSelector : public Selector {
private:
	char* value;
	StringOption* option;
	int* currentItem;
	int optionsize;
public: 
	StringSelector (const char* question, int* selectedItem, StringOption* options);
	bool increase(void);
	bool decrease(void);
	char* getValue(void);
};

class TimeSelector : public Selector {
private:
	char* time;
	unsigned long* minutes;
	int minute_steps;
	int mintime;
	int maxtime;
public: 
	~TimeSelector();
	TimeSelector (const char* question, unsigned long* tijd, int minute_steps);
	bool increase(void);
	bool decrease(void);
	char* getValue(void);
};


class MenuItem {
private:
	int item_id;
	const char* item;
	bool is_sub;
	Menu* sub;
	MenuItem* next;
	MenuItem* previous;
	Selector* selector;
public:
	MenuItem(int id, const char* txt, bool issub, Menu* submenu, MenuItem *last, Selector* sel);
	void setNext(MenuItem* item);
	MenuItem* getPrevious(void);
	MenuItem* getNext(void);
	bool hasNext(void);
	bool hasPrevious(void);
	int getID(void);
	const char* getText(void);
	bool isSubMenu(void);
	Menu* getMenu(void);
	Selector* getSelector(void);
};

class Menu {
private:
	uint8_t selpin;
	MenuItem *start;
	MenuItem *current;
	MenuItem *last;
	bool selected;
	bool fullRepaintNeeded;
	
public:
	Menu(void);
	Menu(uint8_t selector_pin);
	void addItem(int id, const char* text, bool is_sub, Menu* sub, Selector* sel);
	void highlightNext(void);
	void highlightPrevious(void);
	void selectCurrent(void);
	void display(void);
	void display(Adafruit_ILI9341 *t);
};


#endif
