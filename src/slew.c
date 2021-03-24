#include <math.h>
#include "slew.h"


void SlewInit(Slew* s, float sample_rate){
	s->sample_rate_ = sample_rate;
	s->rate_ = .001f;
	s->val_ = 0.f;
}

float SlewProcess(Slew* s, float in){
	s->val_ += (in - s->val_) * s->rate_;
	return s->val_;
}

void SlewSetRate(Slew* s, float seconds){
	s->rate_ = 1.f / (seconds * s->sample_rate_);
}