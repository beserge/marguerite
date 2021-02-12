#pragma once
#ifndef MRG_ADSR_H
#define MRG_ADSR_H

#include <stdbool.h>
#include <stdint.h>

/** Distinct stages that the phase of the envelope can be located in.
- IDLE   = located at phase location 0, and not currently running
- ATTACK  = First segment of envelope where phase moves from 0 to 1
- DECAY   = Second segment of envelope where phase moves from 1 to SUSTAIN value
- SUSTAIN = Third segment of envelope, stays at SUSTAIN level until GATE is
released
- RELEASE =     Fourth segment of envelop where phase moves from SUSTAIN to 0
- LAST    =  Last segment, aka release
*/
enum {
  ADSR_SEG_IDLE,
  ADSR_SEG_ATTACK,
  ADSR_SEG_DECAY,
  ADSR_SEG_SUSTAIN,
  ADSR_SEG_RELEASE,
  ADSR_SEG_LAST,
};

typedef struct {
  float sus_, seg_time_[ADSR_SEG_LAST], a_, b_, y_, x_, prev_, atk_time_;
  int sample_rate_;
  uint8_t mode_;
} Adsr;

void AdsrInit(Adsr *a, float sample_rate);
float AdsrProcess(Adsr *a, bool gate);
float Tau2Pole(Adsr *a, float tau);
float AdsrFilter(Adsr *a);

#endif