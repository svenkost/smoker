#include "Menu.h"

const char* Selector::getQuestion(void) {
	return question;
};


FloatSelector::~FloatSelector(void) {
	free(temp);
};

FloatSelector::FloatSelector(const char* q, float* value, float step, float minimum, float maximum) {
	temp = (char*) malloc(10*sizeof(char));
	min = minimum;
	max = maximum;
	question = q;
	selected = value;
	stepping = step;
}

bool FloatSelector::increase(void) {
	bool retval = false;
	if (*selected+stepping > max) {
		*selected = max;
		retval = true;
	} else {
		*selected += stepping;
	}
	return retval;
}

bool FloatSelector::decrease(void) {
	bool retval = false;
	if (*selected-stepping < min) {
		*selected = min;
		retval = true;
	} else {
		*selected -= stepping;
	}
	return retval;
}

char* FloatSelector::getValue(void) {
	*temp = '\0';
	dtostrf(*selected, 5, 1, temp);
	return temp;
}

DoubleSelector::~DoubleSelector(void) {
	free(temp);
};

DoubleSelector::DoubleSelector(const char* q, double* value, double step, double minimum, double maximum) {
	temp = (char*) malloc(10*sizeof(char));
	min = minimum;
	max = maximum;
	question = q;
	selected = value;
	stepping = step;
}

bool DoubleSelector::increase(void) {
	bool retval = false;
	if (*selected+stepping > max) {
		*selected = max;
		retval = true;
	} else {
		*selected += stepping;
	}
	return retval;
}

bool DoubleSelector::decrease(void) {
	bool retval = false;
	if (*selected-stepping < min) {
		*selected = min;
		retval = true;
	} else {
		*selected -= stepping;
	}
	return retval;
}

char* DoubleSelector::getValue(void) {
	*temp = '\0';
	dtostrf(*selected, 5, 1, temp);
	return temp;
}



IntSelector::~IntSelector(void) {
	free(temp);
};

IntSelector::IntSelector(const char* q, int* value, int step, int minimum, int maximum) {
	temp = (char*) malloc(10*sizeof(char));
	min = minimum;
	max = maximum;
	question = q;
	selected = value;
	stepping = step;
}

bool IntSelector::increase(void) {
	bool retval = false;
	if (*selected+stepping > max) {
		*selected = max;
		retval = true;
	} else {
		*selected += stepping;
	}
	return retval;
}

bool IntSelector::decrease(void) {
	bool retval = false;
	if (*selected-stepping < min) {
		*selected = min;
		retval = true;
	} else {
		*selected -= stepping;
	}
	return retval;
}

char* IntSelector::getValue(void) {
	*temp='\0';
	sprintf( temp, "%d", *selected );
	return temp;
}

StringOption::StringOption(const char* text) {
	option = text;
	next = NULL;
	id=0;
};

int StringOption::getID(void) {
	return id;
};

void StringOption::setID(int i) {
	id=i;
};

const char* StringOption::getOption(void) {
	return option;
};

void StringOption::setNext(StringOption* nxt, int nr) {
	next = nxt;
	next->setID(nr);
};

StringOption* StringOption::getNext(void) {
	return next;
};

StringOption* StringOption::addOption(const char* text) {
	StringOption *opt = new StringOption (text);
	StringOption* last;
	
	last = this;
	
	while (last->getNext() != NULL) {
		last = last->getNext();
	}
	last->setNext(opt, id+1);
	return this;
};

const char* StringOption::getOptionByID(int i) {
	StringOption *cur = this;
	bool found = false;
	
	while (!found && cur != NULL) {
		found = (cur->getID() == i);
		if (!found) {
			cur = cur->getNext();
		};
	};
	if (cur != NULL) {
		return cur->getOption();
	} else {
		return NULL;
	};
};

StringSelector::StringSelector(const char* q, int* selectedItem, StringOption* options) {
	value = (char*) malloc(20*sizeof(char));
	question = q;
	option = options;
	currentItem=selectedItem;
	optionsize = 1;
	StringOption* lastitem = option;
	while(lastitem->getNext() != NULL) {
		optionsize++;
		lastitem = lastitem->getNext();
	};	
};

bool StringSelector::increase(void) {
	bool retval = false;
	
	if (*currentItem == (optionsize-1)) { // now loop to the beginning
		*currentItem = 0;
		retval = true;
	}  else {
		*currentItem = *currentItem+1;
	}
	return retval;
};

bool StringSelector::decrease(void) {
	bool retval = false;
	
	if ((*currentItem) == 0) {//now loop to the end
		retval = true;
		*currentItem = optionsize-1;
	} else {
		*currentItem = *currentItem-1;
	}
	return retval;	
};

char* StringSelector::getValue(void) {
	const char *val = option->getOptionByID(*currentItem);
	strcpy (value, val); 
	return value;
};

TimeSelector::TimeSelector (const char* q, unsigned long* tijd, int minutesteps){
	time = (char*) malloc(10*sizeof(char));
	question = q;
	minute_steps = minutesteps;
	mintime = 15;
	maxtime = 24*60;
	minutes = tijd;
};

TimeSelector::~TimeSelector() {
	free (time);
}

bool TimeSelector::increase(void) {
	bool retval = false;
	
	if (*minutes + minute_steps > maxtime) {
		*minutes = maxtime;
		retval = true;
	} else {
		*minutes += minute_steps;
	}
	return retval;	
};

bool TimeSelector::decrease(void) {
	bool retval = false;
	
	if (*minutes - minute_steps < mintime) {
		*minutes = mintime;
		retval = true;
	} else {
		*minutes -= minute_steps;
	}
	return retval;	
};

char* TimeSelector::getValue(void) {
	sprintf(time,"%02d:%02d", *minutes/60, *minutes%60);
	return time;
}

MenuItem::MenuItem(int id, const char* txt, bool issub, Menu* submenu, MenuItem* last, Selector* sel) {
	
	selector = sel;
	item_id = id;
	item = txt;
	is_sub = issub;
	sub = submenu;
	if (is_sub) {
		sub = submenu;
	}
	previous = last;
	next = NULL;
	if (last != NULL) {
		previous->setNext(this);
	};
};

void MenuItem::setNext(MenuItem* item) {
	next = item;
};

MenuItem* MenuItem::getNext(void) {
	return next;
};

MenuItem* MenuItem::getPrevious(void) {
	return previous;
};

int MenuItem::getID(void) {
	return item_id;
};

const char* MenuItem::getText(void) {
	return item;
};

bool MenuItem::isSubMenu(void) {
	return is_sub;
};

Menu* MenuItem::getMenu(void) {
	return sub;
};

Selector* MenuItem::getSelector(void) {
	return selector;
};

bool MenuItem::hasNext(void) {
	return (next != NULL);
};

bool MenuItem::hasPrevious(void) {
	return (previous != NULL);
};	

int getInterruptNrFromPinNr (uint8_t pin) {
	int interrupt=-1;
	
	switch (pin) {
		case 2:
			interrupt=0;
			break;
		case 3:
			interrupt=1;
			break;
		case 18:
			interrupt=5;
			break;
		case 19:
			interrupt=4;
			break;
		case 20:
			interrupt=3;
			break;
		case 21:
			interrupt=2;
			break;
		}
	return interrupt;
}

void buttonPressed() {
  // write the interrupt routine.
  Serial.println("button pressed");
}
	
Menu::Menu(void) {
	start = NULL;
	current = NULL;
	last = NULL;
	selected=false;
	fullRepaintNeeded=false;
}	
	
Menu::Menu(uint8_t selector_pin) {
	selpin = selector_pin;
	pinMode(selpin, INPUT);
	attachInterrupt(getInterruptNrFromPinNr(selpin), buttonPressed, FALLING);
	start = NULL;
	current = NULL;
	last = NULL;
	selected=false;
	fullRepaintNeeded=false;
};

void Menu::addItem(int id, const char* text, bool is_sub, Menu* sub, Selector* sel) {
	if (is_sub && id != -1) { // Don't do this for a 'back' item
		sub->addItem(-1, "back\0", true, this, NULL);
	}
	MenuItem *item = new MenuItem(id, text, is_sub, sub, last, sel);
	if (start == NULL) {
		start = item;
	};
	if (current == NULL) {
		current = item;
	};
	last = item;
};

void Menu::highlightNext(void) {
	if (selected && !current->isSubMenu()) {
		Selector *sel = current->getSelector();
		sel->increase();	
	} else {
		if (selected && current->isSubMenu()) {
			current->getMenu()->highlightNext();
		} else {
			if (current->hasNext()) {
				current = current->getNext();
			} else { // wrap to the begin
				current = start;
			};
		};
	};
};


void Menu::highlightPrevious(void) {
	if (selected && !current->isSubMenu()) {
		Selector *sel = current->getSelector();
		sel->decrease();	
	} else {
		if (selected && current->isSubMenu()) {
			current->getMenu()->highlightPrevious();
		} else {
	
			if (current->hasPrevious()) {
				current = current->getPrevious();
			} else { // wrap to the end
				current = last;
			};
		};
	};
};

void Menu::display(Adafruit_ILI9341 *t) {

	if (fullRepaintNeeded) {
		fullRepaintNeeded = false;
		t->fillRect(0,12,320,100,ILI9341_WHITE);
	}
	t->setTextColor(ILI9341_BLUE);
	t->setTextSize(1);
	t->setCursor(0,12);
	// Is there an item selected right now?
	if (selected && !current->isSubMenu()) {
		t->fillRect(0,12,320,24,ILI9341_WHITE);
		t->setCursor(0,12);

		Selector *sel = current->getSelector();
		t->print(sel->getQuestion());
		t->print("(");
		t->print(sel->getValue());
		t->println(")");
	} else {
		// Is there a submenu selected?
		if (selected && current->isSubMenu()) {
			current->getMenu()->display(t);
		} else { // nothing selected
			t->fillRect(0,12,18,100,ILI9341_WHITE);

			MenuItem *item;
			int selected_id = current->getID();

			item = start;
			while (item!=NULL) {
				if (item->getID() == selected_id) {
					t->print(" * ");
				} else {
					t->print("   ");
				}

				t->print(item->getText());

				if (item->getSelector() != NULL) {
					t->print("(");
					t->print(item->getSelector()->getValue());
					t->print(")");
				}
				if (item->isSubMenu()) {
					t->print(" >");
				}
				t->println('\n');
				item = item->getNext();
			};
		};
	};
};


void Menu::display(void) {
	if (selected && !current->isSubMenu()) {
		Selector *sel = current->getSelector();
		Serial.print(sel->getQuestion());
		Serial.print("(");
		Serial.print(sel->getValue());
		Serial.println(")");
	} else {
		if (selected && current->isSubMenu()) {
			current->getMenu()->display();
		} else {
			MenuItem *item;
			int selected_id = current->getID();
	
			item = start;
			while (item!=NULL) {
				if (item->getID() == selected_id) {
					Serial.print(" * ");
				} else {
					Serial.print("   ");
				}

				Serial.print(item->getText());
		
				if (item->getSelector() != NULL) {
					Serial.print("(");
					Serial.print(item->getSelector()->getValue());
					Serial.print(")");
				}
				if (item->isSubMenu()) {
					Serial.print(" >");
				}
				Serial.print("\n");	
				item = item->getNext();
			};
		};
	};
};


void Menu::selectCurrent(void) {
	fullRepaintNeeded = true;
	if (!selected) {
		selected = true;
	} else {
		if (current->isSubMenu() && current->getID() != -1) {
			current->getMenu()->selectCurrent();
		} else {
			selected = !selected;
		}
	};
}
