#include "dsp.h"
#include "syntheticsnaredrum.h"

void SyntheticSnareDrumInit(SyntheticSnareDrum* ssd, float sample_rate)
{
    ssd->sample_rate_ = sample_rate;

    ssd->phase_[0]        = 0.0f;
    ssd->phase_[1]        = 0.0f;
    ssd->drum_amplitude_  = 0.0f;
    ssd->snare_amplitude_ = 0.0f;
    ssd->fm_              = 0.0f;
    ssd->hold_counter_    = 0;
    ssd->sustain_gain_    = 0.0f;

	ssd->sustain_ = false;
	ssd->accent_ = .6f;
	ssd->f0_ = 200.f / sample_rate;
	ssd->fm_amount_ = .1f;
	ssd->decay_ = .3f;
	ssd->snappy_ = .7f;
	
    ssd->trig_ = false;

	ssd->even_ = true;

    SvfInit(&ssd->drum_lp_,  sample_rate);
    SvfInit(&ssd->snare_hp_, sample_rate);
    SvfInit(&ssd->snare_lp_, sample_rate);
}

float SSDDistortedSine(float phase)
{
    float triangle = (phase < 0.5f ? phase : 1.0f - phase) * 4.0f - 1.3f;
    return 2.0f * triangle / (1.0f + fabsf(triangle));
}

float SyntheticSnareDrumProcess(SyntheticSnareDrum* ssd, bool trigger)
{
    const float decay_xt = ssd->decay_ * (1.0f + ssd->decay_ * (ssd->decay_ - 1.0f));
    const float drum_decay
        = 1.0f
          - 1.0f / (0.015f * ssd->sample_rate_)
                * powf(2.f,
                       kOneTwelfth
                           * (-decay_xt * 72.0f - ssd->fm_amount_ * 12.0f
                              + ssd->snappy_ * 7.0f));

    const float snare_decay
        = 1.0f
          - 1.0f / (0.01f * ssd->sample_rate_)
                * powf(2.f, kOneTwelfth * (-ssd->decay_ * 60.0f - ssd->snappy_ * 7.0f));
    const float fm_decay = 1.0f - 1.0f / (0.007f * ssd->sample_rate_);

    float snappy = ssd->snappy_ * 1.1f - 0.05f;
    snappy       = fclamp(snappy, 0.0f, 1.0f);

    const float drum_level  = sqrtf(1.0f - snappy);
    const float snare_level = sqrtf(snappy);

    const float snare_f_min = fmin(10.0f * ssd->f0_, 0.5f);
    const float snare_f_max = fmin(35.0f * ssd->f0_, 0.5f);

    SvfSetFreq(&ssd->snare_hp_, snare_f_min * ssd->sample_rate_);
    SvfSetFreq(&ssd->snare_lp_, snare_f_max * ssd->sample_rate_);
    SvfSetRes(&ssd->snare_lp_, 0.5f + 2.0f * snappy);

    SvfSetFreq(&ssd->drum_lp_, 3.0f * ssd->f0_ * ssd->sample_rate_);

    if(trigger || ssd->trig_)
    {
        ssd->trig_            = false;
        ssd->snare_amplitude_ = ssd->drum_amplitude_ = 0.3f + 0.7f * ssd->accent_;
        ssd->fm_                                = 1.0f;
        ssd->phase_[0] = ssd->phase_[1] = 0.0f;
        ssd->hold_counter_
            = (int)((0.04f + ssd->decay_ * 0.03f) * ssd->sample_rate_);
    }

    ssd->even_ = !ssd->even_;
    if(ssd->sustain_)
    {
        ssd->sustain_gain_ = ssd->snare_amplitude_ = ssd->accent_ * ssd->decay_;
        ssd->drum_amplitude_                  = ssd->snare_amplitude_;
        ssd->fm_                              = 0.0f;
    }
    else
    {
        // Compute all D envelopes.
        // The envelope for the drum has a very long tail.
        // The envelope for the snare has a "hold" stage which lasts between
        // 40 and 70 ms
        ssd->drum_amplitude_
            *= (ssd->drum_amplitude_ > 0.03f || ssd->even_) ? drum_decay : 1.0f;
        if(ssd->hold_counter_)
        {
            --ssd->hold_counter_;
        }
        else
        {
            ssd->snare_amplitude_ *= snare_decay;
        }
        ssd->fm_ *= fm_decay;
    }

    // The 909 circuit has a funny kind of oscillator coupling - the signal
    // leaving Q40's collector and resetting all oscillators allow some
    // intermodulation.
    float reset_noise        = 0.0f;
    float reset_noise_amount = (0.125f - ssd->f0_) * 8.0f;
    reset_noise_amount       = fclamp(reset_noise_amount, 0.0f, 1.0f);
    reset_noise_amount *= reset_noise_amount;
    reset_noise_amount *= ssd->fm_amount_;
    reset_noise += ssd->phase_[0] > 0.5f ? -1.0f : 1.0f;
    reset_noise += ssd->phase_[1] > 0.5f ? -1.0f : 1.0f;
    reset_noise *= reset_noise_amount * 0.025f;

    float f = ssd->f0_ * (1.0f + ssd->fm_amount_ * (4.0f * ssd->fm_));
    ssd->phase_[0] += f;
    ssd->phase_[1] += f * 1.47f;
    if(reset_noise_amount > 0.1f)
    {
        if(ssd->phase_[0] >= 1.0f + reset_noise)
        {
            ssd->phase_[0] = 1.0f - ssd->phase_[0];
        }
        if(ssd->phase_[1] >= 1.0f + reset_noise)
        {
            ssd->phase_[1] = 1.0f - ssd->phase_[1];
        }
    }
    else
    {
        if(ssd->phase_[0] >= 1.0f)
        {
            ssd->phase_[0] -= 1.0f;
        }
        if(ssd->phase_[1] >= 1.0f)
        {
            ssd->phase_[1] -= 1.0f;
        }
    }

    float drum = -0.1f;
    drum += SSDDistortedSine(ssd->phase_[0]) * 0.60f;
    drum += SSDDistortedSine(ssd->phase_[1]) * 0.25f;
    drum *= ssd->drum_amplitude_ * drum_level;

    SvfProcess(&ssd->drum_lp_, drum);
    drum = ssd->drum_lp_.low_;

    float noise = rand() * kRandFrac;
    SvfProcess(&ssd->snare_lp_, noise);
    float snare = ssd->snare_lp_.low_;
    SvfProcess(&ssd->snare_hp_, snare);
    snare = ssd->snare_hp_.high_;
    snare = (snare + 0.1f) * (ssd->snare_amplitude_ + ssd->fm_) * snare_level;

    return snare + drum; // It's a snare, it's a drum, it's a snare drum.
}