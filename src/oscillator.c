#include "dsp.h"
#include "oscillator.h"

static inline float Polyblep(float phase_inc, float t);

float TWO_PI_RECIP = 1.0f / TWOPI_F;

void OscillatorInit(Oscillator* osc, float sample_rate)
{
	osc->sr_        = sample_rate;
	osc->sr_recip_  = 1.0f / sample_rate;
	osc->amp_       = 0.5f;
	osc->phase_     = 0.0f;
	osc->phase_inc_ = OscillatorCalcPhaseInc(osc, 100.f);
	osc->waveform_  = WAVE_SIN;
	osc->eoc_       = true;
	osc->eor_       = true;
	osc->last_out_ = 0.f;
}

float OscillatorProcess(Oscillator* osc)
{
    float out, t;
    switch(osc->waveform_)
    {
        case WAVE_SIN: out = sinf(osc->phase_); break;
        case WAVE_TRI:
            t   = -1.0f + (2.0f * osc->phase_ * TWO_PI_RECIP);
            out = 2.0f * (fabsf(t) - 0.5f);
            break;
        case WAVE_SAW:
            out = -1.0f * (((osc->phase_ * TWO_PI_RECIP * 2.0f)) - 1.0f);
            break;
        case WAVE_RAMP: out = ((osc->phase_ * TWO_PI_RECIP * 2.0f)) - 1.0f; break;
        case WAVE_SQUARE: out = osc->phase_ < PI_F ? (1.0f) : -1.0f; break;
        case WAVE_POLYBLEP_TRI:
            t   = osc->phase_ * TWO_PI_RECIP;
            out = osc->phase_ < PI_F ? 1.0f : -1.0f;
            out += Polyblep(osc->phase_inc_, t);
            out -= Polyblep(osc->phase_inc_, fmodf(t + 0.5f, 1.0f));
            // Leaky Integrator:
            // y[n] = A + x[n] + (1 - A) * y[n-1]
            out       = osc->phase_inc_ * out + (1.0f - osc->phase_inc_) * osc->last_out_;
            osc->last_out_ = out;
            break;
        case WAVE_POLYBLEP_SAW:
            t   = osc->phase_ * TWO_PI_RECIP;
            out = (2.0f * t) - 1.0f;
            out -= Polyblep(osc->phase_inc_, t);
            out *= -1.0f;
            break;
        case WAVE_POLYBLEP_SQUARE:
            t   = osc->phase_ * TWO_PI_RECIP;
            out = osc->phase_ < PI_F ? 1.0f : -1.0f;
            out += Polyblep(osc->phase_inc_, t);
            out -= Polyblep(osc->phase_inc_, fmodf(t + 0.5f, 1.0f));
            out *= 0.707f; // ?
            break;
        default: out = 0.0f; break;
    }
    osc->phase_ += osc->phase_inc_;
    if(osc->phase_ > TWOPI_F)
    {
        osc->phase_ -= TWOPI_F;
        osc->eoc_ = true;
    }
    else
    {
        osc->eoc_ = false;
    }
    osc->eor_ = (osc->phase_ - osc->phase_inc_ < PI_F && osc->phase_ >= PI_F);

    return out * osc->amp_;
}

float OscillatorCalcPhaseInc(Oscillator* osc, float f)
{
    return (TWOPI_F * f) * osc->sr_recip_;
}

static float Polyblep(float phase_inc, float t)
{
    float dt = phase_inc * TWO_PI_RECIP;
    if(t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if(t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    else
    {
        return 0.0f;
    }
}