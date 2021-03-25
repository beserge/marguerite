#include "variableshapeosc.h"

float VariableShapeOscInit(float sr, VariableShapeOsc* osc){
    osc->sample_rate_ = sr;
	
    osc->master_phase_ = 0.0f;
    osc->slave_phase_  = 0.0f;
    osc->next_sample_  = 0.0f;
    osc->previous_pw_  = 0.5f;
    osc->high_         = false;

	osc->master_frequency_ = 440.f / sr;
	osc->waveshape_ = 0.f;
	osc->pw_ = 0.f;
	osc->enable_sync_ = false;
	osc->slave_frequency_ = 220.f / sr;
}

float ComputeNaiveSample(float phase,
                                                  float pw,
                                                  float slope_up,
                                                  float slope_down,
                                                  float triangle_amount,
                                                  float square_amount)
{
    float saw    = phase;
    float square = phase < pw ? 0.0f : 1.0f;
    float triangle
        = phase < pw ? phase * slope_up : 1.0f - (phase - pw) * slope_down;
    saw += (square - saw) * square_amount;
    saw += (triangle - saw) * triangle_amount;
    return saw;
}

float VariableShapeOscProcess(VariableShapeOsc* osc){
	float next_sample = osc->next_sample_;

    bool  reset                   = false;
    bool  transition_during_reset = false;
    float reset_time              = 0.0f;

    float this_sample = next_sample;
    next_sample       = 0.0f;

    const float square_amount   = fmax(osc->waveshape_ - 0.5f, 0.0f) * 2.0f;
    const float triangle_amount = fmax(1.0f - osc->waveshape_ * 2.0f, 0.0f);
    const float slope_up        = 1.0f / (osc->pw_);
    const float slope_down      = 1.0f / (1.0f - osc->pw_);

    if(osc->enable_sync_)
    {
        osc->master_phase_ += osc->master_frequency_;
        if(osc->master_phase_ >= 1.0f)
        {
            osc->master_phase_ -= 1.0f;
            reset_time = osc->master_phase_ / osc->master_frequency_;

            float slave_phase_at_reset
                = osc->slave_phase_ + (1.0f - reset_time) * osc->slave_frequency_;
            reset = true;
            if(slave_phase_at_reset >= 1.0f)
            {
                slave_phase_at_reset -= 1.0f;
                transition_during_reset = true;
            }
            if(!(osc->high_) && slave_phase_at_reset >= osc->pw_)
            {
                transition_during_reset = true;
            }
            float value = ComputeNaiveSample(slave_phase_at_reset,
                                             osc->pw_,
                                             slope_up,
                                             slope_down,
                                             triangle_amount,
                                             square_amount);
            this_sample -= value * ThisBlepSample(reset_time);
            next_sample -= value * NextBlepSample(reset_time);
        }
    }

    osc->slave_phase_ += osc->slave_frequency_;
    while(transition_during_reset || !reset)
    {
        if(!(osc->high_))
        {
            if(osc->slave_phase_ < osc->pw_)
            {
                break;
            }
            float t = (osc->slave_phase_ - osc->pw_)
                      / (osc->previous_pw_ - osc->pw_ + osc->slave_frequency_);
            float triangle_step = (slope_up + slope_down) * osc->slave_frequency_;
            triangle_step *= triangle_amount;

            this_sample += square_amount * ThisBlepSample(t);
            next_sample += square_amount * NextBlepSample(t);
            this_sample -= triangle_step * ThisIntegratedBlepSample(t);
            next_sample -= triangle_step * NextIntegratedBlepSample(t);
            osc->high_ = true;
        }

        if(osc->high_)
        {
            if(osc->slave_phase_ < 1.0f)
            {
                break;
            }
            osc->slave_phase_ -= 1.0f;
            float t             = osc->slave_phase_ / osc->slave_frequency_;
            float triangle_step = (slope_up + slope_down) * osc->slave_frequency_;
            triangle_step *= triangle_amount;

            this_sample -= (1.0f - triangle_amount) * ThisBlepSample(t);
            next_sample -= (1.0f - triangle_amount) * NextBlepSample(t);
            this_sample += triangle_step * ThisIntegratedBlepSample(t);
            next_sample += triangle_step * NextIntegratedBlepSample(t);
            osc->high_ = false;
        }
    }

    if(osc->enable_sync_ && reset)
    {
        osc->slave_phase_ = reset_time * osc->slave_frequency_;
        osc->high_        = false;
    }

    next_sample += ComputeNaiveSample(osc->slave_phase_,
                                      osc->pw_,
                                      slope_up,
                                      slope_down,
                                      triangle_amount,
                                      square_amount);
    osc->previous_pw_ = osc->pw_;


    osc->next_sample_ = next_sample;
    return (2.0f * this_sample - 1.0f);
}