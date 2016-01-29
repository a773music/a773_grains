/*
Alternate firmware for the ginky synthese grains eurorack module
Code by a773 (atte.dk) and released under the GPL licence
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define TRIGGER_LENGTH 10
#define UPPER_POT       2
#define MIDDLE_POT      1
#define LOWER_POT       0
#define CLOCK_IN        3
#define UPPER_POT_MAX   500
#define MIDDLE_POT_MAX   500
#define LOWER_POT_MAX   500
#define NB_POT_SLICES 4
#define MODE_SIMPLE 0
#define MODE_COMPLEX 1
#define PWM_PIN       11

long variable_trigger_length;
long last_trigger_in = 0;
long last_trigger_out = 0;
long now = 0;
long time_between_ins = 0;
long time_between_outs = 0;

boolean in_clock_high = false;
boolean stopped = false;
boolean out_clock_high = false;
boolean edge = false;

int mode = -1;
float factor = 0;

int simple_factors[10] = {1,2,4,8,16,32,64,128,256,512};
int complex_factors[10] = {1,3,5,7,11,13,17,19,23,29};


void setup() 
{
  //clock_high = false;
}


int slice2factor(int slice, int mode){
  if(mode==MODE_SIMPLE){
    return simple_factors[slice];
  } else {
    return complex_factors[slice];
  }
}

int get_mult(int mode){
  int slice = analogRead(UPPER_POT) * (NB_POT_SLICES-1) / UPPER_POT_MAX;
  return slice2factor(slice,mode);
}

int get_div(int mode){
  int slice = analogRead(MIDDLE_POT) * (NB_POT_SLICES-1) / MIDDLE_POT_MAX;
  return slice2factor(slice,mode);
}

long get_time(){
  return millis();
}

void loop() 
{
  int gate = analogRead(CLOCK_IN);
  int reset = analogRead(LOWER_POT);
  
  now = get_time();
  edge = false;
  // detect gate in
  if (gate > 32) {
    if (!in_clock_high) {
      edge = true;
      if(last_trigger_in != 0){
        time_between_ins = now - last_trigger_in;
      }
      last_trigger_in = now;
    }
    in_clock_high = true;
  } else {
    in_clock_high = false;
  }
  

  // detect reset/mode
  if (reset < LOWER_POT_MAX/3) {
    // CCW simple mode
    if(edge){
      mode = MODE_SIMPLE;
      stopped = false;
    }
  } else if(reset > LOWER_POT_MAX/3*2){
    // CW complex mode
    if(edge){
      mode = MODE_COMPLEX;
      stopped = false;
    }
  } else {
    // stopped
    stopped = true;
    last_trigger_out = 0;
  }

  // setup mult and div
  if(edge){
    time_between_outs = int(time_between_ins * get_div(mode) / get_mult(mode));
  }




  // trigger?
  if(time_between_ins > 0 && !stopped){
     if(last_trigger_out == 0 && edge){
       analogWrite(PWM_PIN,255);
       out_clock_high = true;
       last_trigger_out = now;
     } else if(now - last_trigger_out >= time_between_outs){
       analogWrite(PWM_PIN,255);
       out_clock_high = true;
       last_trigger_out = now;
     }
  }
  
  // reduce trigger length if going real fast
  if(time_between_outs <= TRIGGER_LENGTH * 2){
    variable_trigger_length = time_between_outs / 2;
  } else {
    variable_trigger_length = TRIGGER_LENGTH;
  }

  // turn trigger out low
  if(now - last_trigger_out > variable_trigger_length && out_clock_high){
    analogWrite(PWM_PIN,0);
    out_clock_high = false;
  }

}

