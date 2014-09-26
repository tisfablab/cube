/* capsense.h

 ****************************************************************************
 WS2812 LED functions
 ****************************************************************************
 capsense_initialize()    touch sensor during init sequence!
 capsense_recalibrate()   recalibrate touch sensor. touch it!
 capsense_read()          returns true if touch is detected
 
 */
 
#include <CapacitiveSensor.h>

#ifndef __LUNA__capsense_h__
#define __LUNA__capsense_h__

// *********************************************
// VARIABLES and CONSTANTS
// *********************************************

// capsense library class
CapacitiveSensor cs = CapacitiveSensor(CAPSENSE_SEND_PIN, CAPSENSE_RECEIVE_PIN);

// constants
const float CAPSENSE_MIN_HYSTERESIS = 40; // don't return true until reached

// variables
long capsense_value = 0;             // to touch or not touch?
long capsense_value_raw = 0;         // unfiltered capsense raw reading
long capsense_value_smooth = 0;      // smoothed value (80% old smooth 20% raw)
long capsense_value_max = 0;         // maximum value
long capsense_value_min = 0;         // minimum value
long capsense_value_limit = 0;       // in the middle of min and max
long capsense_value_hysteresis = 0;  // safety distance from limit

// *********************************************
bool capsense_read() {
// *********************************************
// read capacity sensor and return 1 if it is a sure touch
// auto-calibrates on first touch
  // read sensor through capsense class
  capsense_value_raw = cs.capacitiveSensorRaw(15);
  // smoothe sensor readings
  capsense_value_smooth = capsense_value_smooth * 0.8 + capsense_value_raw * 0.2;
  if (capsense_value_max==0) 
    capsense_value_max = capsense_value_raw;
  else if (capsense_value_raw > capsense_value_limit + capsense_value_hysteresis) 
    capsense_value_max = capsense_value_max * 0.9 + capsense_value_raw * 0.1;
  if (capsense_value_min==0) 
    capsense_value_min = capsense_value_raw;
  else if (capsense_value_raw < capsense_value_limit - capsense_value_hysteresis) 
    capsense_value_min = capsense_value_min * 0.9 + capsense_value_raw * 0.1;
    capsense_value_limit = (capsense_value_max + capsense_value_min) / 2;
    capsense_value_hysteresis = abs(capsense_value_max - capsense_value_min) / 5;
  if (capsense_value_hysteresis > CAPSENSE_MIN_HYSTERESIS) {
    // hysteresis ok, min-max values are reliable
    if (capsense_value_smooth > capsense_value_limit + capsense_value_hysteresis) {
    // we're pretty sure you're touching the sensor - HIGH
    capsense_value = 1;
   }
    else if (capsense_value_smooth < capsense_value_limit - capsense_value_hysteresis) {
    // you're not touching the sensor - LOW
    capsense_value = 0;
  }
  // else: leave capsense_value unchanged
  }
  return capsense_value;
}

// *********************************************
void capsense_initialize() {
// *********************************************
// inialization actually doesn't do anything but forcing you to touch the 
// sensor in order to to autocalibrate values
  capsense_value_limit = capsense_value_max = capsense_value_min = 0;
  capsense_value_smooth = capsense_value_hysteresis = capsense_value = 0;
  while (!capsense_read()) {
    effect_blink(64, 0, 0, 200);
    delay(20);
  }
  while (capsense_read()) {
  led_setRGB(0, 64, 0);
    delay(20);
  }
}  

// *********************************************
void capsense_recalibrate() {
// *********************************************
// same as capsense_initialize();
  capsense_initialize();
}

#endif // __LUNA__capsense_h__
