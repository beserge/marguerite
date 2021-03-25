#include "ramposc.h"

float RampOscInit(RampOsc* osc, float sr){
	osc->phase_ = 0.f;
	osc->phase_inc_ = 2.f * 440.f / sr; //-1 to 1
	osc->sr_ = sr;
}

float RampOscProcess(RampOsc* osc){
	osc->phase_ += osc->phase_inc_;
	if(osc->phase_ >= 1.f) osc->phase_ -= 2.f;
	return osc->phase_;
}