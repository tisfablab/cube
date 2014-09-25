/* led.h

 ****************************************************************************
 WS2812 LED functions
 ****************************************************************************
 led_initialize()        inizialize led / call from setup()
 led_setRGB(r, g, b)     set led to rgb color(r/g/b=0..255)
 led_setHSV(h, s, v)     set led to hsv color(h=0..360, s/v=0..1)
 
 */
 
#include <Adafruit_NeoPixel.h>

#ifndef __LUNA__led_h__
#define __LUNA__led_h__

// start adafruit's neopixel library class
Adafruit_NeoPixel led(1, PIN_WS2811, NEO_RGB + NEO_KHZ800);
 
// *********************************************
void led_initialize() {
// *********************************************
// initialize all pixels to 'off'
  led.begin();
  led.show(); 
}
 
// *********************************************
void led_setRGB(byte r, byte g, byte b) {
// *********************************************  
// set RGB values and send to LED
// r/g/b = 0..255
  led.setPixelColor(0, r, g, b);
  led.show();
}

// *********************************************
void led_setHSV(float h, float s, float v) {
// *********************************************  
// Set HSV color values and send to LED
// h = 0..360
// s = 0..1.0
// v = 0..1.0
  int i;
  float r, g, b;
  float f, p, q, t;
  if( s == 0 ) // achromatic (grey)
    r = g = b = v;
  else {
    h /= 60;      // sector 0 to 5
    i = floor( h );
    f = h - i;      // factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      default: // case 5:
        r = v;
        g = p;
        b = q;
        break;
    }  
  }
  led.setPixelColor(0, 255*r, 255*g, 255*b);
  led.show();
}

#endif // __LUNA__led_h__
