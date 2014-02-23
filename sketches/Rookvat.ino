//Own libraries
#include <Menu.h>
#include <RGBLed.h>
#include <Speaker.h>
#include <GasValve.h>
#include <TemperatureSensors.h>
#include <Logger.h>
#include <MyTFT.h>

//3rd party libraries
#include <Encoder.h>

#include <DallasTemperature.h>
#include <OneWire.h>

#include <mmc.h>
#include <tinyFAT.h>

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>

#include <Servo.h>
#include <Stepper.h>

const int TARGET_TEMP_BARREL = 0;
const int TARGET_TEMP_MEAT = 1;

const int menuSelectorRE_A = 19;
const int menuSelectorRE_B = 27;

const int menuButton = 18;
const int pauseButton = A3;

const int oneWireTempSensors_pin = 3;

const int gasValveMotorRotationCheck_RE_A = A2;
const int gasValveMotorRotationCheck_RE_B = 38;

const int gasValveMotorA = 23;
const int gasValveMotorB = 24;
const int gasValveMotorC = 25;
const int gasValveMotorD = 26;

const int speaker_pin = 37;

const int led1_R = 12;
const int led1_G = 11;
const int led1_B = 10;

const int smokeValveServoPin = 9;
const int smokeValveMin = 0;
const int smokeValveMax = 180;

const int smokerRegulationInterval = 60; // Every minute. Hope it is ok. 

const int meatTempPin = A4;
const int barrelTempPin = A5;

const int TFT_CS = 53;
const int TFT_DC = 49;
const int TFT_MOSI = 51;
const int TFT_MISO = 50;
const int TFT_CLK = 52;
const int TFT_RST = 48;
const int SD_CS = 47;

/* Initialize the one wire sensors*/
TemperatureSensors tempSensors(oneWireTempSensors_pin, barrelTempPin, meatTempPin);

/* Initialize the RGB Led */
RGBLed led1(led1_R,led1_G,led1_B);

/* Initialize the speaker */
Speaker speaker(speaker_pin);

/* Initialize the Rotary Encoders */
Encoder menuSelector(menuSelectorRE_A, menuSelectorRE_B);
Encoder gasValveMotorRotationCheck(gasValveMotorRotationCheck_RE_A, gasValveMotorRotationCheck_RE_B);

/* Initialize the LCD Panel */
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
MyTFT *tft;

/* Initialize the RTC (Real Time Clock) */
// nothing?!

/* Initialize the gas valve; it uses a rotary encoder to check if the gas valve is at the end */
GasValve gasValve(&gasValveMotorRotationCheck, gasValveMotorA, gasValveMotorB, gasValveMotorC, gasValveMotorD);

/* Initialize the servo */
SmokeValve *smokeValve; 

/* Initialize the menu */
Menu *mainMenu = new Menu(menuButton);

/* Initialize the logger to the SD card*/
Logger logger(SD_CS);

/* Here are all the parameters for the application */
float target_temp = 30.0; // The target temperature 
int target_temp_location = TARGET_TEMP_BARREL;
int rooktijd=120; //smoketime in minutes

int current_hour=10;
int current_minute=30;
int current_second=30;
int current_month=2;
int current_day=3;
int current_year=2014;

int smoking_started = 0; //Is the barrel up-and-running, or are we in 'setup mode'?
long encvalue=0; // used for the menu-rotary encoder
volatile bool wasInSelected=false;

time_t starttime, lastregulation; // times for start and lastregulation
boolean firstSmokeLoop=true;
float lasttemp, lasttempdif;

/*
 *Here is all the setup of the project done
 */
void setup() {
  // initialize Serial device to support debugging
  Serial.begin(9600);

  //initialize the LCD
  tft = new MyTFT(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
  //initialize the menu
  createMenu();
  
  //Close the smoke valve with the servo
  smokeValve = new SmokeValve(smokeValveServoPin, smokeValveMin, smokeValveMax);
  smokeValve->fullyClose();

  //Close the gasvalve
  gasValve.fullyClose();

  //blink light to indicate start
  led1.changeColor(&COLOR_GREEN);
  delay(100);
  led1.changeColor(&COLOR_BLUE);
  delay(100);  
  led1.changeColor(&COLOR_RED);
  //test speaker
  speaker.on();
  delay(20);
  speaker.off();  

  // Initialize the rotary encoder
  encvalue = menuSelector.read();

  // Initialize the menubutton
  pinMode(menuButton, INPUT);
  attachInterrupt(getInterruptForPin(menuButton), bPressed, FALLING);

  // Create a new file
  logger.createUniqueFileName();
}

/*
 * This is the main of the arduino application
 */
void loop() {
  long gasValvePos, smokeValvePos;
  float barrelTemp, meatTemp, ambientTemp;


  tmElements_t tm;

  long newvalue;
  bool selected = false;
  int steps = 0;

  // Get the time from the RTC
  if (RTC.read(tm)) {
    current_hour=tm.Hour;
    current_minute=tm.Minute;
    current_second=tm.Second;
    current_month=tm.Month;
    current_day=tm.Day;
    current_year=tmYearToCalendar(tm.Year); 
  };

  // Do menu things if necessary
  mainMenu->display(tft->getTFT());
  wasInSelected = false;
  selected=false;
  while (!selected && !wasInSelected) {
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
    } 
    else {
      if (newvalue < encvalue) {
        steps = (encvalue - newvalue)/4;
        for (int i=0;i<steps;i++) {
          mainMenu->highlightPrevious();
        };
        encvalue=newvalue;
        selected=true;
      } 
      else {
        delay(100);
        //the same so wait for a new value
      }
    }
  }

  // Check the temps
  barrelTemp = tempSensors.getBarrelTemperature();
  meatTemp = tempSensors.getMeatTemperature();
  ambientTemp = tempSensors.getAmbientTemperature();

  RTC.read(tm);
  // display the temps and time at the LCD
  tft->displayTemps(ambientTemp, meatTemp, barrelTemp, target_temp, target_temp_location == TARGET_TEMP_MEAT);
  tft->displayTime(&tm);
//  tft->displayTimeRemaining(seconds_smoking_left, smoking_started);

  // Now check if the smoking is started
  if (smoking_started) {
    if (firstSmokeLoop) {
      logger.writeln("Smoking started");
      starttime = RTC.get();
      // now determine the endtime for the smoking.
      firstSmokeLoop = false;
      lasttemp = 0;
      lasttempdif = 0;
      // Change led to green to indicate that smoking is started
      led1.changeColor(&COLOR_GREEN);
    }
    // Do all the regulating one every x seconds. Else there is to much turning on all the knobs
    time_t current = RTC.get();
    if (current - lastregulation > smokerRegulationInterval) {

      // Check the temps
      barrelTemp = tempSensors.getBarrelTemperature();
      meatTemp = tempSensors.getMeatTemperature();
      ambientTemp = tempSensors.getAmbientTemperature();
      if (ambientTemp > target_temp) {
        logger.writeln("Error: ambient temperature higher than target temperature. Aborting!");
        led1.changeColor(&COLOR_MAGENTA);
        smoking_started = false;
      }
      // Get the positions of the gasvalve steppermotor and smoke valve servo
      gasValvePos = gasValve.getCurrentPosition();
      smokeValvePos = smokeValve->getCurrentPosition();

      // Also update the LCD display to show the current positions (temps are already done
      tft->displayValveStatus(smokeValvePos, gasValvePos);
      
      // Now determine which temperature to use and see what the diff is between the current temp and the target temp
      float temp = (target_temp_location == TARGET_TEMP_BARREL)?barrelTemp:meatTemp;
      float tempdif = target_temp - temp;

      // Now write all the logging for later inspection
      logger.writeLoggingInfo(ambientTemp, meatTemp, barrelTemp, target_temp, target_temp_location, temp, tempdif, lasttempdif, gasValvePos, smokeValvePos);

      // tempdif > 0 betekent dat de temperatuur nog niet bereikt is. Er mag nog doorgestookt worden, 
      // alleen moet als de tempdif bijna bij 0 is wel opgelet worden dat de gaskraan niet meer hoog open blijft.

      if (tempdif > 0) { // is it too cold. but how much? And was it to cold last time?
        logger.writeln("1: tempdif > 0");
        if (lasttempdif < 0) { // was it to hot last time?
          logger.writeln("1.1: lasttempdif < 0");
          if (tempdif < 1) { // is it just 1 degree to cold? Try to regulate it with the smoke valve
            logger.writeln("1.1.1: tempdif < 1");
            smokeValve->close(50); // close it and hope the temp will stabilize
          } else if (tempdif < 3) { // 3 degrees to cold, but last time it was too hot, so the temp dropped a lot
            logger.writeln("1.1.2: tempdif < 3");
            smokeValve->fullyClose(); // close the smokevalve fully. 
            gasValve.open(1); // just 1 step open
          } else { // more than 3 degrees temp drop in 1 minute. That is really strange. 
            logger.writeln("1.1.3: tempdif >= 3");
            smokeValve->fullyClose(); // close the smokevalve
            gasValve.open(3); // turn on the heat
          }
        } else { // it was not to hot last time
          logger.writeln("1.2: lasttempdif >= 0");
          // now determine how close we are
          if (tempdif < 5) { // just 5 degrees? Look at the last value, so we know how much it was raised in 1 cycle
            logger.writeln("1.2.1: tempdif < 5");
            if (lasttempdif <= tempdif) { // it even got colder?
              logger.writeln("1.2.1.1: lasttempdif <= tempdif");
              gasValve.open(1); // open the gas a little bit
              smokeValve->close(5); // and close the smokevalve;
            } else if (lasttempdif < 10) { // it was 10 degrees to cold and now 5. So lets temper the fire and hope it will use the restwarmte
              logger.writeln("1.2.1.2: lasttempdif < 10");
              if (gasValve.close(5)) { // now fully closed?
                // do something with the smokevalve? I don't know 
               logger.writeln("1.2.1.2.1: gasvalve.close == true");
             } 
            } else if (lasttempdif < 20) {
              logger.writeln("1.2.1.3: lasttempdif < 20");

              // So it was almost 20 degrees in 1 cycle
              if (gasValve.isFullyClosed()) { // is the gasvalve already closed and it still raised that much?
               logger.writeln("1.2.1.3.1: gasValve.isFullyClosed()");
               smokeValve->fullyOpen();
              } else {
               logger.writeln("1.2.1.3.2: !gasValve.isFullyClosed()");
                gasValve.fullyClose(); // 
                smokeValve->open(20);
              }
            } else { // more than 20 degrees in one cycle. So vent it and hope the temp will not raise to much
              logger.writeln("1.2.1.4: lasttempdif >= 20");
              gasValve.fullyClose();
              smokeValve->fullyOpen();
            }
          } else if (tempdif < 10) { // just 10 degrees to cold
              logger.writeln("1.2.2: tempdif < 10");
             if (lasttempdif < 10) { // but last time it was also 10 degrees to cold
               logger.writeln("1.2.2.1: tempdif < 10");
               if (gasValve.open(3)) {// turn on the gas
                 logger.writeln("1.2.2.1.1: gasvalve.open == true");
                 // it is now fully open. 
                 smokeValve->fullyClose(); // so close the smokevalve, if opened
               }
             } else {
               logger.writeln("1.2.2.2: tempdif >= 10");
               gasValve.close(3); // close the gas a little bit so it won't rise to fast
             }
           
          } else if (tempdif < 30) { // 30 degrees to cold. 
            logger.writeln("1.2.3: tempdif < 30");
            if (lasttempdif < 30) { // but last time it was also 30 degrees to cold
              logger.writeln("1.2.3.1: lasttempdif < 30");
              if (gasValve.open(5)) {// turn on the gas
                logger.writeln("1.2.3.1.1: gasvalve.open == true");
                // it is now fully open. 
                smokeValve->fullyClose(); // so close the smokevalve, if opened
              }
            } else {
              logger.writeln("1.2.3.2: lasttempdif >= 30");
              gasValve.close(1); // close the gas a little bit so it won't rise to fast
            }
              
          } else {
            logger.writeln("1.2.4: tempdif >= 30");
            gasValve.fullyOpen();
            smokeValve->fullyClose();
          }
        }
        
      }



      // als tempdif < 0 betekent dat de max-temp overschreden is. Afhankelijk van de grootte van de overschrijding
      // zal de gaskraan helemaal dicht moeten en de rookklep open. 
      if (tempdif < 0) {
        logger.writeln("2: tempdif < 0");
        if (lasttempdif < 0) { // Hij was vorige keer ook al te warm.
          logger.writeln("2.1: lasttempdif < 0");
          // now check the position of the gasvalve
          if (gasValve.isFullyClosed()) { // the GasValve is already totally closed
            logger.writeln("2.1.1: gasValve.isFullyClosed()");
            // So check the smokeValve
            if (smokeValve->isFullyOpen()) { // Is the smokevalve totally open?
              logger.writeln("2.1.1.1: smokeValve->isFullyClosed()");
              // I don't know what to do now. Gas is off, smokevalve totally open. Just wait for the temp to drop.
            } else {
              logger.writeln("2.1.1.2: !smokeValve->isFullyClosed()");
              if (tempdif > -1) { // just 1 degree to hot?
                logger.writeln("2.1.1.2.1: tempdif > -1");
                smokeValve->open(1); // just 1 step extra open
              } else if (tempdif >-3) {
                logger.writeln("2.1.1.2.2: tempdif > -3");
                smokeValve->open(5); // 5 steps extra open
              } else { // More than 3 degrees to hot? Open the smokevalve totally
                logger.writeln("2.1.1.2.3: tempdif <= -3");
                smokeValve->fullyOpen();
              }
            }
          } else { // the gasvalve is not fully closed, but the temp is to hot. Just close it now
            logger.writeln("2.1.2: !gasValve.isFullyClosed()");
            gasValve.fullyClose();

            if (tempdif >-3) { // 3 degrees to hot? Open the smokevalve so the temp can drop faster
              logger.writeln("2.1.2.1: tempdif > -3");
              smokeValve->open(10); // 10 steps extra open
            } else { // More than 3 degrees to hot? Open the smokevalve totally
              logger.writeln("2.1.2.2: tempdif <= -3");
              smokeValve->fullyOpen();
            }
          }
        } else { // last time it was not to hot, so it is just to hot
          logger.writeln("2.2: lasttempdif >= 0");
          if (gasValve.isFullyClosed()) { // is the gasvalve already closed?
            logger.writeln("2.2.1: gasValve.isFullyClosed()");
            if (smokeValve->isFullyOpen()) { // is the smokevalve already open?
              logger.writeln("2.2.1.1: smokeValve->isFullyOpen()");
              // Don't know what to do now. Both controls are exhausted 
            } else { // only the smokeValve is usable to regulate the temperature
              logger.writeln("2.2.1.2: !smokeValve->isFullyOpen()");
              if (tempdif > -1) { // just 1 degree to hot?
                logger.writeln("2.2.1.2.1: tempdif > -1");
                smokeValve->open(1); // just 1 step extra open
              } else if (tempdif >-3) {
                logger.writeln("2.2.1.2.2: tempdif > -3");
                smokeValve->open(5); // 5 steps extra open
              } else { // More than 3 degrees to hot? Open the smokevalve totally
                logger.writeln("2.2.1.2.3: tempdif <= -3");
                smokeValve->fullyOpen();
              }
            }
          } else { // the gasvalve is not fully closed yet.             
           logger.writeln("2.2.2: !gasValve.isFullyClosed()");
           if (tempdif > -1) { // just 1 degree to hot?
              logger.writeln("2.2.2.1: tempdif > -1");
              if (gasValve.close(2)) { // close 2 steps, but is it totally closed now?
                logger.writeln("2.2.2.1.1: gasvalve.close == true");
                smokeValve->open(5); // Open more, because gas is closed now
              }  else {
                logger.writeln("2.2.2.1.2: gasvalve.close != true");
                smokeValve->open(1); // just 1 step extra open
              }
            } else if (tempdif >-3) {
              logger.writeln("2.2.2.2: tempdif > -3");
              if (gasValve.close(5)) {
                logger.writeln("2.2.2.2.1: gasvalve.close == true");
                smokeValve->open(20);
              } else {
                logger.writeln("2.2.2.2.2: gasvalve.close != true");
                smokeValve->open(5); // 5 steps extra open
              }
            } else { // More than 3 degrees to hot? Open the smokevalve totally
              logger.writeln("2.2.2.3: tempdif <= -3");
              gasValve.fullyClose();
              smokeValve->fullyOpen();
            }
          }
        }
      }

      // store the current temps for the next round, so we can see how fast the temp is raising or dropping  
      lasttemp = temp;
      lasttempdif = tempdif;

      // now make sure the interval is reset
      lastregulation = RTC.get();
      // now check if the endtime is reached
      long seconds_smoking_left = determineSmokingTimeLeftInSeconds(starttime, lastregulation, rooktijd);
      tft->displayTimeRemaining(seconds_smoking_left, smoking_started);
      if (seconds_smoking_left<0) {
        smoking_started=false;
        firstSmokeLoop=true;
        logger.writeln("Smoking stopped because end time is reached");
        for (int l = 0;l<10;l++) {
          led1.changeColor(&COLOR_BLUE);
          speaker.on();
          delay(500);
          led1.changeColor(&COLOR_GREEN);
          speaker.off();  
          delay(500);  
        } // for
        led1.changeColor(&COLOR_GREEN);
      } // end time reached?
    } // regulator cycle loop
  } // smoking started
} // main loop

/*
 * calculates the remaining time in seconds
 */
long determineSmokingTimeLeftInSeconds(time_t starttime, time_t currenttime, int smoketime_in_minutes) {
  long timeSmokerActiveInSeconds = currenttime - starttime;
  long totalSmokingTimeInSeconds = (long)smoketime_in_minutes * 60l;

  return totalSmokingTimeInSeconds - timeSmokerActiveInSeconds; 
}

/* 
 * this method writes the value to the console if the board is connected.
 */
void debug(char* str) {
  logger.writeln(str);
}

/*
 * This is the initialization code for the menu-class. This can be controlled by a rotary encoder and a button
 */
void createMenu() {
  StringOption* opt = new StringOption("Vat\0");
  opt->addOption("Vlees\0");

  StringOption* startopties = new StringOption("Nee\0");
  startopties->addOption("Ja\0");

  FloatSelector *tempsel = new FloatSelector("Stel temperatuur in:\0", &target_temp, 0.5f, 10, 250);
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
  mainMenu->addItem(2, "Doel temperatuur\0", false, NULL, tempsel);
  mainMenu->addItem(3, "Vlees of vat sensor\0", false, NULL, valuesel);
  mainMenu->addItem(4, "Rooktijd\0", false, NULL, timesel);
  mainMenu->addItem(5, "Instellingen\0", true, instellingen, NULL);
}

/*
The interrupt routine for the button
 */
void bPressed() {
  mainMenu->selectCurrent();
  wasInSelected = true;
};

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


