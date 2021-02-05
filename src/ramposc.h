#pragma once
#ifndef MRG_RAMPOSC_H
#define MRG_RAMPOSC_H

#include <stdint.h>

typedef struct{
	float phase_;     //current phase -1.f to 1.f
	float phase_inc_; //how much to inc every time process is called
	float sr_;        //engine samplerate
} RampOsc;

float RampOscInit(float sr, RampOsc* osc);
float RampOscProcess(RampOsc* osc);

#endif