#ifndef MRG_SYNTHSD_H
#define MRG_SYNTHSD_H

#include "svf.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    float sample_rate_;

    bool  trig_;
    bool  sustain_;
    float accent_, f0_, fm_amount_, decay_, snappy_;

    float phase_[2];
    float drum_amplitude_;
    float snare_amplitude_;
    float fm_;
    float sustain_gain_;
    int   hold_counter_;

	bool even_;

    Svf drum_lp_;
    Svf snare_hp_;
    Svf snare_lp_;
} SyntheticSnareDrum;

void SyntheticSnareDrumInit(SyntheticSnareDrum* ssd, float sample_rate);

float SyntheticSnareDrumProcess(SyntheticSnareDrum* ssd, bool trigger);

float SSDDistortedSine(float phase);

#endif