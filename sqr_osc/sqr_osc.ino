
/*
Square wave oscillator
Alternate firmware for the ginky synthese grains eurorack module
Code by a773 (atte.dk) and released under the GPL licence

Calibration:
1) with the lowest pot turned all the way CCW, send 0V into input 3
2) the pitch should be C2 (65Hz) 
3) with 1V into input 3, adjust the value of VOCT_SCALE until the pitch
   is one octave higher, C3 (130Hz), higher values makes the pitch higher
4) with 0V intor input turn lowest pot all the CW, adjust KNOB_SCALE until
   the pitch is one octave higher, C3 (130Hz), higher values makes the pitch
   higher
5) optionally adjust BASE_OCTAVE to shift octaves up (3 is one octave higher)
*/

//calibration
#define VOCT_SCALE    1.0
#define KNOB_SCALE    1.0
#define BASE_OCTAVE   2 // base octave not lower than 2

#define PWM_PIN       11
#define TUNE_KNOB      0
#define TUNE_KNOB_RANGE 596

#define VOCT          3
#define NB_VOLTS 9

float freq;
float volt, volt1, volt2, volt3, volt4, volt5, volt6, volt7, volt8;

float volt2freq(float volt){
  return 440 / pow(2, 4.75) * pow(2, min(volt,3) + BASE_OCTAVE); 
}


void setup() {
  volt = (1.0 * analogRead(TUNE_KNOB)/TUNE_KNOB_RANGE) + (analogRead(VOCT)/220.2);
  volt1 = volt;
  volt2 = volt;
  volt3 = volt;
  volt4 = volt;
  volt5 = volt;
  volt6 = volt;
  volt7 = volt;
  volt8 = volt;
}

void loop() {
  volt = (1.0 * analogRead(TUNE_KNOB)/TUNE_KNOB_RANGE*KNOB_SCALE) + (analogRead(VOCT)/220.2*VOCT_SCALE);
  
  freq = volt2freq((volt + volt1 + volt2 + volt3 + volt4 + volt5 + volt6 + volt7 + volt8)/NB_VOLTS);
  tone(PWM_PIN,freq);
  volt8 = volt7;
  volt7 = volt6;
  volt6 = volt5;
  volt5 = volt4;
  volt4 = volt3;
  volt3 = volt2;
  volt2 = volt1;
  volt1 = volt;
       
}
