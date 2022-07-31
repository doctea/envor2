#ifndef CLOCK__INCLUDED
#define CLOCK__INCLUDED
unsigned long last_ticked = 0;

extern unsigned long effective_TIME_MULT;

unsigned long long bpm_clock () {
  return millis() * effective_TIME_MULT; //millis();
}

#endif