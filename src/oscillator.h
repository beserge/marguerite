#pragma once
#ifndef MRG_OSCILLATOR_H
#define MRG_OSCILLATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "dsp.h"

/** Choices for output waveforms, POLYBLEP are appropriately labeled. Others are naive forms.
*/
enum
{
	WAVE_SIN,
	WAVE_TRI,
	WAVE_SAW,
	WAVE_RAMP,
	WAVE_SQUARE,
	WAVE_POLYBLEP_TRI,
	WAVE_POLYBLEP_SAW,
	WAVE_POLYBLEP_SQUARE,
	WAVE_LAST,
};

typedef struct{
	uint8_t waveform_;
	float   amp_;
	float   sr_, sr_recip_, phase_, phase_inc_;
	float   last_out_, last_freq_;
	bool    eor_, eoc_;
} Oscillator;

void OscillatorInit(Oscillator* osc, float sample_rate);
float OscillatorProcess(Oscillator* osc);
float OscillatorCalcPhaseInc(Oscillator* osc, float f);

#endif