#include "adsr.h"
#include <math.h>

float Tau2Pole(Adsr *a, float tau) {
  return expf(-1.0f / (tau * a->sample_rate_));
}

float AdsrFilter(Adsr *a) {
  a->y_ = a->b_ * a->x_ + a->a_ * a->y_;
  return a->y_;
}

void AdsrInit(Adsr *a, float sample_rate) {
  a->seg_time_[ADSR_SEG_ATTACK] = 0.1f;
  a->seg_time_[ADSR_SEG_DECAY] = 0.1f;
  a->sus_ = 0.7f;
  a->seg_time_[ADSR_SEG_RELEASE] = 0.1f;
  // timer_ = 0;
  a->a_ = 0.0f;
  a->b_ = 0.0f;
  a->x_ = 0.0f;
  a->y_ = 0.0f;
  a->prev_ = 0.0f;
  a->atk_time_ = a->seg_time_[ADSR_SEG_ATTACK] * sample_rate;
  a->sample_rate_ = sample_rate;
}

float AdsrProcess(Adsr *a, bool gate) {
  float pole, out;
  out = 0.0f;

  if (gate && a->mode_ != ADSR_SEG_DECAY) {
    a->mode_ = ADSR_SEG_ATTACK;
    // timer_ = 0;
    pole = Tau2Pole(a, a->seg_time_[ADSR_SEG_ATTACK] * 0.6f);
    a->atk_time_ = a->seg_time_[ADSR_SEG_ATTACK] * a->sample_rate_;
    a->a_ = pole;
    a->b_ = 1.0f - pole;
  } else if (!gate && a->mode_ != ADSR_SEG_IDLE) {
    a->mode_ = ADSR_SEG_RELEASE;
    pole = Tau2Pole(a, a->seg_time_[ADSR_SEG_RELEASE]);
    a->a_ = pole;
    a->b_ = 1.0f - pole;
  }

  a->x_ = (int)gate;
  a->prev_ = (int)gate;

  switch (a->mode_) {
  case ADSR_SEG_IDLE:
    out = 0.0f;
    break;
  case ADSR_SEG_ATTACK:
    out = AdsrFilter(a);

    if (out > .99f) {
      a->mode_ = ADSR_SEG_DECAY;
      pole = Tau2Pole(a, a->seg_time_[ADSR_SEG_DECAY]);
      a->a_ = pole;
      a->b_ = 1.0f - pole;
    }
    break;
  case ADSR_SEG_DECAY:
  case ADSR_SEG_RELEASE:
    a->x_ *= a->sus_;
    out = AdsrFilter(a);
    if (out <= 0.01f)
      a->mode_ = ADSR_SEG_IDLE;
  default:
    break;
  }
  return out;
}
