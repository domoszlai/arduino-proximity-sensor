
// https://github.com/domoszlai/arduino-frequency-counter
#include "frequency_counter_TC.h"

#define DEBUG

#define MAX_FREQUENCY_DROP        500
#define MAX_EXPECTED_FREQ_NOISE   30
#define CL_WEIGHT                 0.75
#define BL_WEIGHT                 0.995

float current_line = 0;
float baseline = 0;

void setup()
{
  pinMode(13, OUTPUT);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif

  current_line = baseline = count_frequency();
}

void loop()
{
  unsigned long f = count_frequency();

  current_line = current_line * CL_WEIGHT + (1 - CL_WEIGHT) * f;
  baseline = baseline * BL_WEIGHT + (1 - BL_WEIGHT) * f;

  // distance is reciprocal, bigger value indicates smaller distance
  // and its range is 0-255 
  int diff = max(0, baseline - current_line);
  if(diff < MAX_EXPECTED_FREQ_NOISE)
  { 
    diff = 0;
  }
  else
  {
    diff -= MAX_EXPECTED_FREQ_NOISE;
  }
  int distance = (diff) * 255L / MAX_FREQUENCY_DROP;
  distance = min(distance, 255);

  analogWrite(13, distance);

#ifdef DEBUG
  Serial.print(distance);
  Serial.print(" ");
  Serial.print(f);
  Serial.print(" ");
  Serial.print(baseline);
  Serial.print(" ");
  Serial.println(current_line);   
#endif  
}

