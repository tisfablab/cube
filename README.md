Glowing Thermometer Cube
===
   Lange Nacht der Forschung / Lunga notte della Ricerca 2014 @ TIS FabLab

===

Important: In order to compile this sketch you will need two additional libraries:
 * http://playground.arduino.cc/Main/CapacitiveSensor
 * https://github.com/adafruit/Adafruit_NeoPixel

You will, of course, also need the Arduino IDE:
 * http://arduino.cc/en/Main/Software (1.0.6)
 
===

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
 
===
 
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


