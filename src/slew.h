#pragma once
#ifndef MRG_SLEW_H
#define MRG_SLEW_H
#include <stdint.h>

typedef struct {
	float rate_;
	float val_;
	float sample_rate_;
} Slew;

void SlewInit(Slew* s, float sample_rate);

float SlewProcess(Slew* s, float in);

//update rate in seconds
void SlewSetRate(Slew* s, float seconds);

#endif
