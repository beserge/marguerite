#include "samplehold.h"

void SampleHoldInit(SampleHold* s){
  s->sample_ = 0.f;
  s->track_ = 0.f;

  s-> previous_ = false;
  s-> trackOn_ = false;
}

float SampleHoldProcess(SampleHold* s, bool trigger, float input){
  if(trigger){
    if(!s->previous_){
      s->sample_ = input;
    }
    s->track_ = input;
  }
  s->previous_ = trigger;

  return s->trackOn_ ? s->track_ : s-> sample_;
}
