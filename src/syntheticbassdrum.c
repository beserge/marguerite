#include "syntheticbassdrum.h"

void SBDClickInit(SBDClick* click, float sample_rate)
{
    click->lp_ = 0.0f;
    click->hp_ = 0.0f;
    SvfInit(&click->filter_, sample_rate);
    SvfSetFreq(&click->filter_, 5000.f);
    SvfSetRes(&click->filter_, 1.f); //2.f
}

float SBDClickProcess(SBDClick* click, float in)
{
    //SLOPE(lp_, in, 0.5f, 0.1f);
    float error = in - click->lp_;
    click->lp_ += (error > 0 ? .5f : .1f) * error;

    fonepole(&click->hp_, click->lp_, 0.04f);
    SvfProcess(&click->filter_, click->lp_ - click->hp_);
    return click->filter_.low_;
}

void SBDAttackNoiseInit(SBDAttackNoise* an)
{
    an->lp_ = 0.0f;
    an->hp_ = 0.0f;
}

float SBDAttackNoiseProcess(SBDAttackNoise* an)
{
    float sample = rand() * kRandFrac;
    fonepole(&an->lp_, sample, 0.05f);
    fonepole(&an->hp_, an->lp_, 0.005f);
    return an->lp_ - an->hp_;
}

void SyntheticBassDrumInit(SyntheticBassDrum* sbd, float sample_rate)
{
    sbd->sample_rate_ = sample_rate;

    sbd->trig_ = false;

    sbd->phase_                = 0.0f;
    sbd->phase_noise_          = 0.0f;
    sbd->f0_                   = 0.0f;
    sbd->fm_                   = 0.0f;
    sbd->fm_lp_                = 0.0f;
    sbd->body_env_lp_          = 0.0f;
    sbd->body_env_             = 0.0f;
    sbd->body_env_pulse_width_ = 0;
    sbd->fm_pulse_width_       = 0;
    sbd->tone_lp_              = 0.0f;
    sbd->sustain_gain_         = 0.0f;

	sbd->new_f0_ = 100.f / sample_rate;
	sbd->f0_ = 0.f;
	sbd->sustain_ = false;
	sbd -> accent_ = .2f;
	sbd->tone_ = .6f;
	sbd->decay_ = .7f;
	sbd->dirtiness_ = .3f;
	sbd->fm_envelope_amount_ = .6f;
	sbd->fm_envelope_decay_ = .3f;

    SBDClickInit(&sbd->click_, sample_rate);
    SBDAttackNoiseInit(&sbd->noise_);
}

float SBDDistortedSine(float phase,
                           float phase_noise,
                           float dirtiness){
    phase += phase_noise * dirtiness;

    //MAKE_INTEGRAL_FRACTIONAL(phase);
    int32_t phase_integral   = (int32_t)phase;
    float   phase_fractional = phase - (float)phase_integral;

    phase            = phase_fractional;
    float triangle   = (phase < 0.5f ? phase : 1.0f - phase) * 4.0f - 1.0f;
    float sine       = 2.0f * triangle / (1.0f + fabsf(triangle));
    float clean_sine = sinf(TWOPI_F * (phase + 0.75f));
    return sine + (1.0f - dirtiness) * (clean_sine - sine);
}

float TransistorVCA(float s, float gain)
{
    s = (s - 0.6f) * gain;
    return 3.0f * s / (2.0f + fabsf(s)) + gain * 0.3f;
}

float SyntheticBassDrumProcess(SyntheticBassDrum* sbd, bool trigger)
{
    float dirtiness = sbd->dirtiness_;
    dirtiness *= fmax(1.0f - 8.0f * sbd->new_f0_, 0.0f);

    const float fm_decay
        = 1.0f
          - 1.0f / (0.008f * (1.0f + sbd->fm_envelope_decay_ * 4.0f) * sbd->sample_rate_);

    const float body_env_decay
        = 1.0f
          - 1.0f / (0.02f * sbd->sample_rate_)
                * powf(2.f, (-sbd->decay_ * 60.0f) * kOneTwelfth);
    const float transient_env_decay = 1.0f - 1.0f / (0.005f * sbd->sample_rate_);
    const float tone_f              = fmin(
        4.0f * sbd->new_f0_ * powf(2.f, (sbd->tone_ * 108.0f) * kOneTwelfth), 1.0f);
    const float transient_level = sbd->tone_;

    if(trigger || sbd->trig_)
    {
        sbd->trig_     = false;
        sbd->fm_       = 1.0f;
        sbd->body_env_ = sbd->transient_env_ = 0.3f + 0.7f * sbd->accent_;
        sbd->body_env_pulse_width_      = sbd->sample_rate_ * 0.001f;
        sbd->fm_pulse_width_            = sbd->sample_rate_ * 0.0013f;
    }

    sbd->sustain_gain_ = sbd->accent_ * sbd->decay_;

    fonepole(&sbd->phase_noise_, rand() * kRandFrac - 0.5f, 0.002f);

    float mix = 0.0f;

    if(sbd->sustain_)
    {
        sbd->f0_ = sbd->new_f0_;
        sbd->phase_ += sbd->f0_;
        if(sbd->phase_ >= 1.0f)
        {
            sbd->phase_ -= 1.0f;
        }
        float body = SBDDistortedSine(sbd->phase_, sbd->phase_noise_, dirtiness);
        mix -= TransistorVCA(body, sbd->sustain_gain_);
    }
    else
    {
        if(sbd->fm_pulse_width_)
        {
            --sbd->fm_pulse_width_;
            sbd->phase_ = 0.25f;
        }
        else
        {
            sbd->fm_ *= fm_decay;
            float fm = 1.0f + sbd->fm_envelope_amount_ * 3.5f * sbd->fm_lp_;
            sbd->f0_      = sbd->new_f0_;
            sbd->phase_ += fmin(sbd->f0_ * fm, 0.5f);
            if(sbd->phase_ >= 1.0f)
            {
                sbd->phase_ -= 1.0f;
            }
        }

        if(sbd->body_env_pulse_width_)
        {
            --sbd->body_env_pulse_width_;
        }
        else
        {
            sbd->body_env_ *= body_env_decay;
            sbd->transient_env_ *= transient_env_decay;
        }

        const float envelope_lp_f = 0.1f;
        fonepole(&sbd->body_env_lp_, sbd->body_env_, envelope_lp_f);
        fonepole(&sbd->transient_env_lp_, sbd->transient_env_, envelope_lp_f);
        fonepole(&sbd->fm_lp_, sbd->fm_, envelope_lp_f);

        float body      = SBDDistortedSine(sbd->phase_, sbd->phase_noise_, dirtiness);
        float transient = SBDClickProcess(&sbd->click_, sbd->body_env_pulse_width_ ? 0.0f : 1.0f)
                          + SBDAttackNoiseProcess(&sbd->noise_);

        mix -= TransistorVCA(body, sbd->body_env_lp_);
        mix -= transient * sbd->transient_env_lp_ * transient_level;
    }

    fonepole(&sbd->tone_lp_, mix, tone_f);
    return sbd->tone_lp_;
}