/*
Alternate firmware for the ginky synthese grains eurorack module
Code by a773 (atte.dk) and released under the GPL licence

Square wave oscillator

Upper knob/in 1:  quantize to semitones, CCW: quantize off, CW quantize on
Middle knob/in 2: unused
Lower knob:       tune (range 1 octave)
In3;              1v/oct pitch input
Out:              Audio out

Calibration:
IMPORTANT: unfortunately you have to power the module from your rack when 
measuring, you can't rely on the tuning when powered from USB, remember to
NOT power from USB and rack at the same time, since this might blow up your module!

1) with the lowest pot turned all the way CCW, send 0V into input 3
2) the pitch should be C2 (65Hz) 
3) with 1V into input 3, adjust the value of VOCT_SCALE until the pitch
   is one octave higher, C3 (130Hz), higher values makes the pitch higher
4) with 0V intor input turn lowest pot all the CW, adjust KNOB_SCALE until
   the pitch is one octave higher, C3 (130Hz), higher values makes the pitch
   higher
5) optionally adjust BASE_OCTAVE to shift octaves up (3 is one octave higher)
*/

// calibration
#define VOCT_SCALE         1.0
#define KNOB_SCALE         1.0
#define BASE_OCTAVE        2 // base octave not lower than 2

// setup
#define PWM_PIN            11
#define VOCT               3
#define TUNE_KNOB          0
#define TUNE_KNOB_RANGE    1024
#define UPPER_POT          2
#define UPPER_KNOB_RANGE   1024
#define OCT_RANGE          5


// stuff used for smoothing
#define NB_SMOOTHING       5
float readings[NB_SMOOTHING];
int readIndex = 0;
float total = 0;
float average = 0;

float freq;
float volt;

float smooth(float value_to_smooth){
  total = total - readings[readIndex];
  readings[readIndex] = value_to_smooth;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= NB_SMOOTHING) {
    readIndex = 0;
  }
  average = total / NB_SMOOTHING;
  return average;
}

float volt2freq(float volt){
  return 440 / pow(2, 4.75) * pow(2, min(volt,OCT_RANGE) + BASE_OCTAVE); 
}

float quantize(float volt){
   return round(volt * 12) / 12.0; 
}

void setup() {
}

void loop() {
  volt = (1.0 * analogRead(TUNE_KNOB)/TUNE_KNOB_RANGE*KNOB_SCALE) + (analogRead(VOCT)/204.0*VOCT_SCALE);
  volt = smooth(volt);
  if(analogRead(UPPER_POT) > UPPER_KNOB_RANGE / 2){
    volt = quantize(volt);
  }
  freq = volt2freq(volt);
  tone(PWM_PIN,freq);
       
}
