/* cube-arduino.ino

 ****************************************************************************
 * Glowing Thermometer Cube                                                 *
 ****************************************************************************
   Lange Nacht der Forschung / Lunga notte della Ricerca 2014 @ TIS FabLab

 The Glowing Thermometer Cube is an Arduino controlled RGB LED inside a 
 translucent white cube. By touching the touch sensitive area (near the bent 
 wire) you can switch between 2 modes:
 
 * in thermometer mode the cube's color reflects the room temperature. Blue 
   means it is cold (default set to 20C), magenta is rather warm (35C), and 
   temperatures in between are interpolated.
 * the second mode is the cube glowing with white light.
 * if you keep touching the sensor area, the led cycles slowly through the 
   rainbow colors and stops when you let go.
   
 * you can define more modes if you want, we already wrote some effects and 
   put the functions into colors.h. Feel free to add your own!
   
 * to switch the lamp on and off simply turn it upside down!
 
 When you power on the lamp it blinks red. Please touch the capacitive sensor
 for 1-2 seconds to calibrate it.
 
 ****************************************************************************
 
 This sketch  works with an Arduino Leonardo with the following components 
 plugged directly into the specified pins:

 * temperature sensor LM35 with flat side out in pin A0, A1 and A2. 
   First pin = +5V of LM35 is A0
 * WS2812 LED 
   - Data out is connected to pin 2 through a 330 Ohm resistor - you will need 
     to solder here
   - +5V and GND go to Arduino's +5V and GND pins near the USB socket - here
     there is soldering needed, too
   - Data out is not used
 * Tilt sensor to pins A4 and A5
 * The touch antenna, which is a bent piece of wire, is connected to pin 12.
   Pin 12 is also connected via a 100pF capacitor to GND (nearby pin).
   Pins 10 and 12 are conneced via a 10MOhm resistor.

 */
 
 
// *********************************************
// CONSTANT DEFINITIONS
// *********************************************

// pins
#define CONTROL_LED_PIN 13          // arduino's builtin LED is on pin 13
#define PIN_WS2811 2                // WS2812LED data pin (Adafruit_NeoPixel)
#define TEMPERATURE_SENSOR_PWR A0   // +5V power supply for LM35 sensor
#define TEMPERATURE_SENSOR_PIN A1   // analog input for temperature sensor
#define TEMPERATURE_SENSOR_GND A2   // ground for LM35 temperature sensor
#define TILT_SENSOR_PIN A4          // pin1 of the tilt sensor
#define TILT_SENSOR_GND A5          // pin2 of the tilt sensor
#define CAPSENSE_SEND_PIN 10        // touch sensor sender pin 
#define CAPSENSE_RECEIVE_PIN 12     // receive pin of the touch sensor

//constants
const float MIN_TEMPERATURE=20.0;   // lower end of temperature interval
const float MIN_HUE=240.0;          // HSV hue for min temperature (240=blue)
const float MAX_TEMPERATURE=35.0;   // upper end of temp. interval
const float MAX_HUE=-60.0;          // HSV hue for max temp (-60=300=magenta)
const int LONGTOUCH_TIME=400;       // millisecs for long-touch
const int TILT_DEBOUNCE=100;        // ignore tilt sensor after status change
const int PRINT_TIME=5000;          // print temperature to serial interval

// mode identifiers
const byte MODE_THERMOMETER=0;
const byte MODE_OFF=80;
const byte MODE_ON=81;
const byte MODE_COLORSELECT=90;
const byte MODE_KEEPCOLOR=91;

// *********************************************
// GLOBAL VARIABLES
// *********************************************

byte currentMode = 0;             // currently active mode
byte oldMode = 0;                 // remember mode for wen you turn it back on
float temperature = 0;            // room temperature 
float hue = 0;                    // color hue that reflects temperature
bool newValue = LOW;              // buffer for input tilt + touch sensors
bool oldTiltValue = LOW;          // used to filter tilt sensor readings
bool currentTouchStatus = LOW;    // HIGH when touch is detected
bool currentTiltStatus = LOW;     // HIGH when cube is upside down (filtered)
unsigned long touchTime = 0;      // time variable to detect long touches
unsigned long tiltLock = 0;       // time variable for filtering tilt signal
unsigned long printTime = 0;      // time variable for temperature output

// *********************************************
// INCLUDE LIBRARIES / EXTERNAL FILES
// ********************************************* 

#include <Adafruit_NeoPixel.h>      // Adafruit's WS2812 LED library
#include "led.h"                    // we manage the led library for you
#include "colors.h"                 // here are some led light effects
#include <CapacitiveSensor.h>       // capacitive touch sensor library
#include "capsense.h"               // we manage the touch sensor as well

// *********************************************
// SETUP FUNCTION
// *********************************************

void setup() {
  Serial.begin(9600);
  // temperature sensor
  analogReference(INTERNAL); // reference for temperatur reading: 2.56V  
  // WARNING: wrong settings can damage temperature sensor and arduino:
  pinMode(TEMPERATURE_SENSOR_PWR, OUTPUT);   
  digitalWrite(TEMPERATURE_SENSOR_PWR, HIGH); // +5v for lm35
  pinMode(TEMPERATURE_SENSOR_GND, OUTPUT);      
  digitalWrite(TEMPERATURE_SENSOR_GND, LOW); // ground for lm35
  // tilt sensor
  pinMode(TILT_SENSOR_PIN, INPUT_PULLUP); // HIGH unless grounded
  pinMode(TILT_SENSOR_GND, OUTPUT);
  digitalWrite(TILT_SENSOR_GND, LOW); // ground for tilt sensor
  // onboard status led
  pinMode(CONTROL_LED_PIN, OUTPUT);
  // WS2812LED
  led_initialize(); // initialize led functions
  // touch sensor
  capsense_initialize(); // please touch sensor to calibrate it
}

// *********************************************
// MAIN LOOP
// *********************************************

void loop() {
  // ********** PROCESS TOUCH SENSOR ***********************************
  if (currentMode!=MODE_OFF) { //only react to touches when lamp is on
    newValue = capsense_read(); // HIGH when your finger touches the antenna
    if (newValue==HIGH) { // you are touching the sensor NOW
      // if newValue differs from currentTouchStatus (=value by the end of the 
      // last loop) it means that you touched the cube just now
      if (newValue!=currentTouchStatus)  
      touchTime = millis(); // remember timestamp for long-touch detection
      else { // you're touching the sensor for a while now...
        if ((millis()-touchTime)>LONGTOUCH_TIME) // is this a "longtouch"?
        currentMode = MODE_COLORSELECT;  // special mode to select color
       }
    }
    else { // you are not touching the sensor
      if (newValue!=currentTouchStatus) { // you let it go just now
        if ((millis()-touchTime)>LONGTOUCH_TIME) // it was a long touch
          currentMode = MODE_KEEPCOLOR; // keep color you set manually
        else // short touch
          currentMode = currentMode + 1; // next mode
      }
    }
    currentTouchStatus = newValue; // remember new touch status for next loop
    // turn the onboard led on when a touch is detected:
    digitalWrite(CONTROL_LED_PIN, currentTouchStatus); 
  }
  // ********** PROCESS TILT SENSOR ************************************
  newValue = digitalRead(TILT_SENSOR_PIN); // HIGH when you tilt the cube 
  if (newValue!=oldTiltValue) { // sensor value has changed
    // the tilt sensor is really sensitive and every vibration produces a
    // short signal spike - we need to smooth that out. also the ball 
    // inside the sensor tends to bounce after tilting, sending on and off 
    // signals repeatedly
    // therefore we ignore signals that are shorter than TILT_DEBOUNCE
    tiltLock = millis() + TILT_DEBOUNCE; 
    oldTiltValue = newValue; // remember current value
  }
  else if (millis()>tiltLock) { // signal is constant for TILT_DEBOUNCE ms
    if (newValue==HIGH) { // cube is upside down
      if (newValue!=currentTiltStatus) { // was standing upright until now
        if (currentMode==MODE_OFF) 
          currentMode = MODE_ON;
        else {
          oldMode = currentMode; // remember mode for when we switch on
          currentMode = MODE_OFF;
        }
      }
	}
    currentTiltStatus = newValue; // remember cube orientation
  }
  
  // ********** DISPLAY CURRENT MODE ***********************************
  switch (currentMode) {
  case 1: // white light
    led_setRGB(255, 255, 255); // 100% red 100% green 100% blue = white
    break;
//  case 2:
//    effect_heartbeat(160, 0, 255);
//    break;
  /* 
    Insert more modes here!
    try the following light effects:
      effect_pulse(255, 255, 255);      // pulsing white light
      effect_candle();                  // candle light effect
      effect_rainbow(3000);             // cycle rainbow colors in 3 secs
      effect_heartbeat(160, 0, 255);    // slow purple heartbeat
      effect_blink(13, 178, 177, 500);  // blinking LuNa turquoise
    ... and then write your own! 
  */
  case MODE_OFF: // special mode: turn off the light ...
    // ... activated by turning the cube upside down
    led_setRGB(0, 0, 0); // 0% red 0% green 0% blue = black = off
    break;
  case MODE_ON: // special mode: turn back on the light ...
    if (oldMode==MODE_KEEPCOLOR) led_setHSV(hue, 1, 1);
      currentMode = oldMode;
    break;  
  case MODE_COLORSELECT: // special mode: slowly change color ...
    // ... activated by long-touching the cube
    hue = effect_rainbow(15000); // cycle through colors in 15 seconds
    break;
  case MODE_KEEPCOLOR: // special mode: don't do anything...
    // ... when you let go the cube after changing colors
    break;
  case MODE_THERMOMETER: // thermometer light
  default: 
    // get temperature and calculate smoothed average
    temperature = (temperature * 0.9) + (temperature_read()*0.1); 
    // limit temperature to interval [min_temperature, max_temperature]
    if (temperature>MAX_TEMPERATURE) 
      temperature = MAX_TEMPERATURE;
    else if (temperature<MIN_TEMPERATURE) 
      temperature = MIN_TEMPERATURE;
    // calculate corresponding color hue (hue as in HSV color spectrum)
    hue = MIN_HUE + (temperature - MIN_TEMPERATURE) * (MAX_HUE - MIN_HUE) / (MAX_TEMPERATURE - MIN_TEMPERATURE);
    if (hue<0) hue = hue + 360; // add 360 if hue angle is negative
    // set led to hsv color
    led_setHSV(hue, 1, 1);
    // make sure mode is 0
    currentMode = 0;
  }

  // ********** PRINT TEMPERATURE VALUE TO SERIAL ************************
  if (millis()-printTime>PRINT_TIME) {
	Serial.print("Temperature: ");
	Serial.print(temperature_read());
	Serial.println(" degC");
	printTime = millis();
  }
  
  // a few relaxing moments for the microprocessor
  delay(5);
}
  
// *********************************************
// FUNCTION read temperature from LM35 sensor
// *********************************************
float temperature_read(void) {
  int value = 0;
  float celsius;
  // read temperature 10 times, take average
  for (int i=0; i<10; i++) 
    value = value + analogRead(TEMPERATURE_SENSOR_PIN);
  value = value / 10;
  value = analogRead(TEMPERATURE_SENSOR_PIN);
  // 0.01V = 1C; 1024 bit; 2,560V reference
  celsius = (value * 256.0 / 1024.0); 
  return celsius;
}
