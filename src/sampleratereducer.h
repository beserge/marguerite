#pragma once
#ifndef MRG_SRR_H
#define MRG_SRR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float frequency_; // 0.f to 1.f. 1.f is full sr
    float phase_;
    float sample_;
    float next_sample_;
    float previous_sample_;
} SampleRateReducer;

void SampleRateReducerInit(SampleRateReducer* s);

float SampleRateReducerProcess(SampleRateReducer* s, float in);

#endif