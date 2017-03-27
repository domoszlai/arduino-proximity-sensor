
// https://github.com/domoszlai/arduino-frequency-counter
#include "frequency_counter_TC.h"

#define DEBUG

// It should be measured, but it is more or less all right for 100pF and 200K
#define MAX_FREQUENCY_DROP        600
// Less noisy -> go closer to 0
#define MAX_EXPECTED_FREQ_NOISE   30
// Less noisy -> go closer to 0
#define CL_WEIGHT                 0.75
// Less noisy -> go closer to 1
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

  // The difference is reciprocal, the bigger indicates smaller distance
  // as frequency drops with proximity
  int diff = max(0, baseline - current_line);
  
  // Force threshold to avoid flickering around 0
  if(diff < MAX_EXPECTED_FREQ_NOISE)
  { 
    diff = 0;
  }
  else
  {
    diff -= MAX_EXPECTED_FREQ_NOISE;
  }

  // Convert the difference to range [0-255]
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

