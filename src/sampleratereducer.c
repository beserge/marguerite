#include <math.h>
#include "sampleratereducer.h"
#include "dsp.h"

void SampleRateReducerInit(SampleRateReducer* s){
    s->frequency_       = .2f;
    s->phase_           = 0.0f;
    s->sample_          = 0.0f;
    s->next_sample_     = 0.0f;
    s->previous_sample_ = 0.0f;
}

float SampleRateReducerProcess(SampleRateReducer* s, float in)
{
    float this_sample = s->next_sample_;
    s->next_sample_      = 0.f;
    s->phase_ += s->frequency_;
    if(s->phase_ >= 1.0f)
    {
        s->phase_ -= 1.0f;
        float t = s->phase_ / s->frequency_;
        // t = 0: the transition occurred right at this sample.
        // t = 1: the transition occurred at the previous sample.
        // Use linear interpolation to recover the fractional sample.
        float new_sample
            = s->previous_sample_ + (in - s->previous_sample_) * (1.0f - t);
        float discontinuity = new_sample - s->sample_;
        this_sample += discontinuity * ThisBlepSample(t);
        s->next_sample_ = discontinuity * NextBlepSample(t);
        s->sample_      = new_sample;
    }
    s->next_sample_ += s->sample_;
    s->previous_sample_ = in;

    return this_sample;
}
