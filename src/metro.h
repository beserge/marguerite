#pragma once
#ifndef MRG_METRO_H
#define MRG_METRO_H
#include <stdint.h>

typedef struct {
  float phs_;
  float phs_inc_;
} Metro;

void MetroInit(Metro *m, float sample_rate);

uint8_t MetroProcess(Metro *m);

#endif
