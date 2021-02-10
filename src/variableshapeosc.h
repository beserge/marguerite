#pragma once
#ifndef MRG_VARISHAPEOSC_H
#define MRG_VARISHAPEOSC_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	float sample_rate_;
    bool  enable_sync_;

    // Oscillator state.
    float master_phase_;
    float slave_phase_;
    float next_sample_;
    float previous_pw_;
    bool  high_;

    // For interpolation of parameters.
    float master_frequency_;
    float slave_frequency_;
    float pw_;
    float waveshape_;
} VariableShapeOsc;

float VariableShapeOscInit(float sr, VariableShapeOsc* osc);
float VariableShapeOscProcess(VariableShapeOsc* osc);

#endif