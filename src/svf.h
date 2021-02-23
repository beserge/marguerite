#pragma once
#ifndef MRG_SVF_H
#define MRG_SVF_H

typedef struct{
float sr_, fc_, res_, drive_, freq_, damp_;
float notch_, low_, high_, band_, peak_;
float input_;
float out_low_, out_high_, out_band_, out_peak_, out_notch_;
float pre_drive_, fc_max_;
} Svf;

void SvfInit(Svf* svf, float sample_rate);
void SvfProcess(Svf* svf, float in);
void SvfSetFreq(Svf* svf, float f);
void SvfSetRes(Svf* svf, float r);
void SvfSetDrive(Svf* svf, float d);

#endif
