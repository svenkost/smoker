
//Own libraries
#include <Menu.h>
#include <RGBLed.h>
#include <Speaker.h>
#include <GasValve.h>
#include <TemperatureSensors.h>
#include <Logger.h>
#include <MyTFT.h>
#include <MyUTFT.h>
#include <Timer5.h>

#include <Encoder.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <AD595.h>

#include <mmc.h>
#include <tinyFAT.h>

#include "SPI.h"
#include <Adafruit_GFX.h> 
#include <Adafruit_ILI9341.h>

#include <memorysaver.h>
#include <UTFT.h>


#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>

#include <Servo.h>
#include <Stepper.h>

#include <Statistic.h>


const int menuSelectorRE_A = 19;
const int menuSelectorRE_B = 27;

const int menuButton = 18;

const int TFT_CS = 53;
const int TFT_DC = 49;
const int TFT_MOSI = 51;
const int TFT_MISO = 50;
const int TFT_CLK = 52;
const int TFT_RST = 48;
const int SD_CS = 47;

const int oneWireTempSensors_pin = 2;
const int meatTempPin = A4;
const int barrelTempPin = A5;
const int barrelTCPin = A6;

const int smokeValveServoPin = 6;
const int smokeValveMin = 0;
const int smokeValveMax = 180;

const int led1_R = 5;
const int led1_G = 4;
const int led1_B = 3;

/* Initialize the one wire sensors*/
TemperatureSensors *tempSensors;

/* Initialize the RGB Led */
RGBLed led1(led1_R,led1_G,led1_B);

/* Initialize the Rotary Encoders */
Encoder menuSelector(menuSelectorRE_A, menuSelectorRE_B);

/* Initialize the menu */
Menu *mainMenu =new Menu();

/* Initialize the LCD Panel */
//Adafruit_ILI9341 *tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
MyUTFT *mytft;

const int TARGET_TEMP_BARREL = 0;
const int TARGET_TEMP_MEAT = 1;

double target_temp=30;
int target_temp_location=0;
int smoking_started=0;
unsigned long rooktijd=600;
int current_hour=10;
int current_minute=30;
int current_month=2;
int current_day=3;
int current_year=2014;
long encvalue=0;

volatile bool wasInSelected=false;
volatile unsigned long lastclick = 0;
volatile bool ledon=false;

void setup() {
  // initialize Serial device to support debugging
  Serial.begin(9600);
  Timer5.initialize();
  Timer5.attachInterrupt(cbLedFlash, 1000000);
  Timer5.start();

  encvalue = menuSelector.read();
  
  tempSensors = new TemperatureSensors(oneWireTempSensors_pin, barrelTempPin, meatTempPin, barrelTCPin);
  mytft = new MyUTFT(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
//  mytft  = new MyTFT(tft);
    //blink light to indicate start
//  led1.changeColor(&COLOR_GREEN);
//  delay(500);
//  led1.changeColor(&COLOR_BLUE);
//  delay(500);  
//  led1.changeColor(&COLOR_RED);

  tmElements_t tm;
  tm.Hour=20;
  tm.Minute=15;
  tm.Second=30;
  tm.Month=3;
  tm.Day=21;
  tm.Year=CalendarYrToTm(2014);   
  mytft->displayTime(&tm);
    
  pinMode(menuButton, INPUT);
  attachInterrupt(getInterruptForPin(18), bPressed, FALLING);
  createMenu();

  
}

void loop() {
  double ambientTemp = tempSensors->getAmbientTemperature();
  double barrelAmbTemp = tempSensors->getOvenAmbientTemperature();
  double meatTemp = tempSensors->getMeatTemperature();
  mytft->displayTemps(ambientTemp, meatTemp, barrelAmbTemp, 123.5f, target_temp, target_temp_location == TARGET_TEMP_MEAT);
  mytft->displayTimeRemaining(rooktijd*60l, smoking_started);
  mytft->displayValveStatus(90, 215);
  
  long newvalue;
  bool selected = false;
  int steps = 0;
    
  mainMenu->display(mytft->getTFT());
  wasInSelected = false;
  selected=false;
  unsigned long tijd = millis();
  while (!selected && !wasInSelected && (millis() - tijd < 5000)) { // every 5 seconds a refresh
    delay(100);
    newvalue = menuSelector.read();
    if ((newvalue - encvalue) % 4 != 0) {
      continue;
    }
    if (newvalue > encvalue) {
      steps = (newvalue - encvalue)/4;
      for (int i=0;i<steps;i++) {
        mainMenu->highlightNext();
      };
      encvalue=newvalue;
      selected=true;
    } else {
      if (newvalue < encvalue) {
        steps = (encvalue - newvalue)/4;
        for (int i=0;i<steps;i++) {
          mainMenu->highlightPrevious();
        };
        encvalue=newvalue;
        selected=true;
      } else {
        delay(100);
        //the same so wait for a new value
      }
    }
  }
  
}



void createMenu() {
  StringOption* opt = new StringOption("Vat\0");
  opt->addOption("Vlees\0");
  
  StringOption* startopties = new StringOption("Nee\0");
  startopties->addOption("Ja\0");
  
  DoubleSelector *tempsel = new DoubleSelector("Stel temperatuur in:\0", &target_temp, 0.5f, 10, 250);
  StringSelector *valuesel = new StringSelector("Meet locatie:\0", &target_temp_location, opt);
  TimeSelector *timesel = new TimeSelector("Rooktijd:\0", &rooktijd, 5);
  StringSelector *smokestartsel = new StringSelector("Nu starten:\0", &smoking_started, startopties);
  
  Menu *instellingen = new Menu();
  
  IntSelector *hoursel = new IntSelector ("Selecteer huidige uur:\0", &current_hour, 1, 0, 23);
  IntSelector *minutesel = new IntSelector ("Selecteer huidige minuut:\0", &current_minute, 1, 0, 59);
  instellingen->addItem(20, "Stel uren in", false, NULL, hoursel);
  instellingen->addItem(21, "Stel minuten in", false, NULL, minutesel);  
  
  IntSelector *daysel = new IntSelector ("Selecteer huidige dag:", &current_day, 1, 1, 31);
  IntSelector *monthsel = new IntSelector ("Selecteer huidige maand:", &current_month, 1, 1, 12);
  IntSelector *yearsel = new IntSelector ("Selecteer huidige jaar:", &current_year, 1, 2014, 2020);
  instellingen->addItem(22, "Stel dag in", false, NULL, daysel);
  instellingen->addItem(23, "Stel maand in", false, NULL, monthsel);
  instellingen->addItem(24, "Stel jaar in", false, NULL, yearsel);  
  
  mainMenu->addItem(1, "Start roken\0", false, NULL, smokestartsel);
  mainMenu->addItem(2, "Eind temperatuur\0", false, NULL, tempsel);
  mainMenu->addItem(3, "Vlees of vat sensor\0", false, NULL, valuesel);
  mainMenu->addItem(4, "Rooktijd\0", false, NULL, timesel);
  mainMenu->addItem(5, "Instellingen\0", true, instellingen, NULL);
  
}

void bPressed() {
  if (millis() > lastclick+500) { // is the last click more than 50 milliseconds old?
    lastclick = millis();
    mainMenu->selectCurrent();
    wasInSelected = true;
  }
}
	

/*
Map the pin to an interrupt number for the Arduino Mega
 */
int getInterruptForPin (uint8_t pin) {
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

void cbLedFlash() {
  if (ledon) {
    led1.changeColor(&COLOR_RED);
  } else {
    led1.changeColor(&COLOR_GREEN);
  }
  ledon = !ledon; 
}

