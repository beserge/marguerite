#pragma once
#ifndef MRG_SYNTHBASSDRUM_H
#define MRG_SYNTHBASSDRUM_H

#include "svf.h"
#include "dsp.h"
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

typedef struct{
    float lp_;
    float hp_;
    Svf   filter_;
} SBDClick;

void SBDClickInit(SBDClick* click, float sample_rate);
float SBDClickProcess(SBDClick* click, float in);



typedef struct{
	float lp_;
    float hp_;
} SBDAttackNoise;

void SBDAttackNoiseInit(SBDAttackNoise* an);
float SBDAttackNoiseProcess(SBDAttackNoise* an);



typedef struct{
	float sample_rate_;

    bool  trig_;
    bool  sustain_;
    float accent_, new_f0_, tone_, decay_;
    float dirtiness_, fm_envelope_amount_, fm_envelope_decay_;

    float f0_;
    float phase_;
    float phase_noise_;

    float fm_;
    float fm_lp_;
    float body_env_;
    float body_env_lp_;
    float transient_env_;
    float transient_env_lp_;

    float sustain_gain_;

    float tone_lp_;

    SBDClick       click_;
    SBDAttackNoise noise_;

    int body_env_pulse_width_;
    int fm_pulse_width_;

} SyntheticBassDrum;

void SyntheticBassDrumInit(SyntheticBassDrum* sbd, float sample_rate);
float SyntheticBassDrumProcess(SyntheticBassDrum* sbd, bool trigger);

float SBDDistortedSine(float phase, float phase_noise, float dirtiness);
float TransistorVCA(float s, float gain);

#endif
