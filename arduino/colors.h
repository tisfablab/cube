/* colors.h

 ****************************************************************************
 LED color effects
 ****************************************************************************
 effect_candle()                candle light effect
 effect_rainbow(ms)             cycle through the rainbow colors
 effect_pulse(r, g, b)          pulsing light
 effect_heartbeat(r, g, b)      slow heartbeat
 effect_blink(r, g, b, ms)      blinking led
 
 */
 
#ifndef __LUNA__colors_h__
#define __LUNA__colors_h__

unsigned long wait_time=0;
// *********************************************
bool wait(int delay) {
// *********************************************
// non-blocking substitute for delay() in loops
// return true after <delay> ms
  if (millis()<wait_time) return false;
  wait_time = millis() + delay;
  return true;
}

int effect_candle_dip = 0;
int effect_candle_dipdelta;
int effect_candle_maxdip;
// *********************************************
void effect_candle(void) {
// *********************************************
// candle light effect
  if (wait(30)) {
    led_setRGB(255, 125-effect_candle_dip, 10); 
    if (effect_candle_dip==0) {
      effect_candle_maxdip = random(10,40);
      effect_candle_dipdelta = effect_candle_maxdip/random(2,5);
      if (random(4)==1) effect_candle_dipdelta=effect_candle_dipdelta*2;
    }
    else if (effect_candle_dip>effect_candle_maxdip) {
      effect_candle_dipdelta = -effect_candle_dipdelta;
    }
    effect_candle_dip = effect_candle_dip + effect_candle_dipdelta;
  }
}  

float effect_rainbow_hue;
unsigned long effect_rainbow_time=0;
// *********************************************  
float effect_rainbow(unsigned long duration) {
// *********************************************  
// cycle through the colors of the rainbow
// one cycle is done in <duration> ms
  if (millis()-effect_rainbow_time>duration) {
    // first call
    effect_rainbow_time=millis();
  }
  if (wait(30)) {
    effect_rainbow_hue = ((millis()-effect_rainbow_time)%duration)*360.0/duration;
    led_setHSV(effect_rainbow_hue, 1.0, 1.0);  
  }
  return effect_rainbow_hue;
}

int effect_pulse_val=128;
int effect_pulse_delta=8;
int effect_pulse_angle=0;
// *********************************************
void effect_pulse(byte r, byte g, byte b, byte inc=6) {
// *********************************************
// pulsing light; inc is how fast the pulsing goes
  if (wait(30)) {
    // intensity is sinus modulated between 55 and 255
    float val = 55+250*sin(3.1415926*effect_pulse_angle/360.0);
    // and then logaritmically scaled - led brightness is not linear!
    val = 1/(1+exp(((val/21)-6)*-1));
    led_setRGB(int(r*val), int(g*val), int(b*val)); 
    effect_pulse_angle += inc;
    if (effect_pulse_angle>=360) effect_pulse_angle=0;
  }
}

unsigned int effect_heartbeat_index = 64;
const unsigned char effect_heartbeat_values[] = {21,21,21,21,21,21,21,21,21,21,21,21,21,22,23,25,28,34,42,54,71,92,117,145,175,203,228,246,255,254,242,220,191,157,121,87,58,34,17,6,1,0,2,5,9,13,16,18,19,20,21,21,21,21,21,21,21,21,21,21,21,21,21,21};
// *********************************************
void effect_heartbeat(byte r, byte g, byte b) {
// *********************************************
// one heartbeat every 5ish seconds.
  if (wait(30)) {
    unsigned int i = effect_heartbeat_index;
    if (i>63) i=0;
    led_setRGB(r*effect_heartbeat_values[i]/255, 
               g*effect_heartbeat_values[i]/255, 
               b*effect_heartbeat_values[i]/255);
    if(++effect_heartbeat_index>=125) effect_heartbeat_index=0;
  }
}

byte effect_blink_flag = 1;
// *********************************************
void effect_blink(byte r, byte g, byte b,int interval=500) {
// *********************************************
// blinks the led. interval=blink speed [ms]
  if (wait(interval)) {
    led_setRGB(r*effect_blink_flag, 
               g*effect_blink_flag, 
               b*effect_blink_flag);
    effect_blink_flag = !effect_blink_flag;
  }
}

#endif // __LUNA__colors_h__
