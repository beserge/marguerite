#pragma once
#ifndef MRG_SAMPLEHOLD_H
#define MRG_SAMPLEHOLD_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
  float sample_; //S&H
  float track_; // T&H
  bool previous_; //edge detection
  bool trackOn_; //true = T&H, false = S&H
} SampleHold;


void SampleHoldInit(SampleHold* s);
float SampleHoldProcess(SampleHold* s, bool trigger, float input);

#endif
