#include <math.h>
#include "svf.h"
#include "dsp.h"
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void SvfInit(Svf* svf, float sample_rate)
{
    svf->sr_        = sample_rate;
    svf->fc_        = 200.0f;
    svf->res_       = 0.5f;
    svf->drive_     = 0.5f;
    svf->pre_drive_ = 0.5f;
    svf->freq_      = 0.25f;
    svf->damp_      = 0.0f;
    svf->notch_     = 0.0f;
    svf->low_       = 0.0f;
    svf->high_      = 0.0f;
    svf->band_      = 0.0f;
    svf->peak_      = 0.0f;
    svf->input_     = 0.0f;
    svf->out_notch_ = 0.0f;
    svf->out_low_   = 0.0f;
    svf->out_high_  = 0.0f;
    svf->out_peak_  = 0.0f;
    svf->out_band_  = 0.0f;
    svf->fc_max_    = svf->sr_ / 3.f;
}

void SvfProcess(Svf* svf, float in)
{
    svf->input_ = in;
    // first pass
    svf->notch_ = svf->input_ - svf->damp_ * svf->band_;
    svf->low_   = svf->low_ + svf->freq_ * svf->band_;
    svf->high_  = svf->notch_ - svf->low_;
    svf->band_  = svf->freq_ * svf->high_ + svf->band_ - svf->drive_ * svf->band_ * svf->band_ * svf->band_;
	svf->band_ = isnan(svf->band_) ? 0.f : svf->band_; 
	
	// take first sample of output
    svf->out_low_   = 0.5f * svf->low_;
    svf->out_high_  = 0.5f * svf->high_;
    svf->out_band_  = 0.5f * svf->band_;
    svf->out_peak_  = 0.5f * (svf->low_ - svf->high_);
    svf->out_notch_ = 0.5f * svf->notch_;
	
    // second pass
    svf->notch_ = svf->input_ - svf->damp_ * svf->band_;
    svf->low_   = svf->low_ + svf->freq_ * svf->band_;
    svf->high_  = svf->notch_ - svf->low_;
    svf->band_  = svf->freq_ * svf->high_ + svf->band_ - svf->drive_ * svf->band_ * svf->band_ * svf->band_;
	svf->band_ = isnan(svf->band_) ? 0.f : svf->band_; 
	
	// average second pass outputs
    svf->out_low_ += 0.5f * svf->low_;
    svf->out_high_ += 0.5f * svf->high_;
    svf->out_band_ += 0.5f * svf->band_;
    svf->out_peak_ += 0.5f * (svf->low_ - svf->high_);
    svf->out_notch_ += 0.5f * svf->notch_;
}

void SvfSetFreq(Svf* svf, float f)
{
    svf->fc_ = fclamp(f, 1.0e-6, svf->fc_max_);
    // Set Internal Frequency for fc_
    svf->freq_ = 2.0f
            * sinf(PI_F
                   * MIN(0.25f,
                         svf->fc_ / (svf->sr_ * 2.0f))); // fs*2 because double sampled
    // recalculate damp
    svf->damp_ = MIN(2.0f * (1.0f - powf(svf->res_, 0.25f)),
                MIN(2.0f, 2.0f / svf->freq_ - svf->freq_ * 0.5f));
}

void SvfSetRes(Svf* svf, float r)
{
    float res = fclamp(r, 0.f, 1.f);
    svf->res_      = res;
    // recalculate damp
    svf->damp_  = MIN(2.0f * (1.0f - powf(svf->res_, 0.25f)),
                MIN(2.0f, 2.0f / svf->freq_ - svf->freq_ * 0.5f));
    svf->drive_ = svf->pre_drive_ * svf->res_;
}

void SvfSetDrive(Svf* svf, float d)
{
    float drv  = fclamp(d * 0.1f, 0.f, 1.f);
    svf->pre_drive_ = drv;
    svf->drive_     = svf->pre_drive_ * svf->res_;
}
