#include "metro.h"
#include "dsp.h"
#include <math.h>

void MetroInit(Metro *m, float sample_rate) {
  m->phs_ = 0.f;
  m->phs_inc_ = 1.f / sample_rate; // 1 Hz
}

uint8_t MetroProcess(Metro *m) {
  m->phs_ += m->phs_inc_;
  if (m->phs_ >= 1.f) {
    m->phs_ -= 1.f;
    return 1;
  }
  return 0;
}