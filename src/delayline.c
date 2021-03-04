#include <math.h>
#include "dsp.h"
#include "delayline.h"

void DelayLineInit(DelayLine* del)
{ 
	DelayLineReset(del); 
}

void DelayLineReset(DelayLine* del)
{
	for(size_t i = 0; i < del->max_size_; i++)
	{
		del->line_[i] = 0.f;
	}
	del->write_ptr_ = 0;
	del->delay_     = 1;
}

void DelayLineSetDelay(DelayLine* del, float delay)
{
	int32_t int_delay = (int32_t)(delay);
	del->frac_             = delay - (float)(int_delay);
	del->delay_ = (size_t)(int_delay) < del->max_size_ ? int_delay
													   : del->max_size_ - 1;
}

void DelayLineWrite(DelayLine* del, float sample)
{
	del->line_[del->write_ptr_] = sample;
	del->write_ptr_        = (del->write_ptr_ - 1 + del->max_size_) % del->max_size_;
}

float DelayLineRead(DelayLine* del)
{
	float a = del->line_[(del->write_ptr_ + del->delay_) % del->max_size_];
	float b = del->line_[(del->write_ptr_ + del->delay_ + 1) % del->max_size_];
	return a + (b - a) * del->frac_;
}

float DelayLineReadLoc(DelayLine* del, float delay)
{
	int32_t delay_integral   = (int32_t)(delay);
	float   delay_fractional = delay - (float)(delay_integral);
	const float a = del->line_[(del->write_ptr_ + delay_integral) % del->max_size_];
	const float b = del->line_[(del->write_ptr_ + delay_integral + 1) % del->max_size_];
	return a + (b - a) * delay_fractional;
}

float DelayLineReadHermite(DelayLine* del, float delay)
{
	int32_t delay_integral   = (int32_t)(delay);
	float   delay_fractional = delay - (float)(delay_integral);

	int32_t     t     = (del->write_ptr_ + delay_integral + del->max_size_);
	const float     xm1   = del->line_[(t - 1) % del->max_size_];
	const float     x0    = del->line_[(t) % del->max_size_];
	const float     x1    = del->line_[(t + 1) % del->max_size_];
	const float     x2    = del->line_[(t + 2) % del->max_size_];
	const float c     = (x1 - xm1) * 0.5f;
	const float v     = x0 - x1;
	const float w     = c + v;
	const float a     = w + v + (x2 - x0) * 0.5f;
	const float b_neg = w + a;
	const float f     = delay_fractional;
	return (((a * f) - b_neg) * f + c) * f + x0;
}

float DelayLineAllpass(DelayLine* del, float sample, size_t delay, float coefficient)
{
	float read  = del->line_[(del->write_ptr_ + delay) % del->max_size_];
	float write = sample + coefficient * read;
	DelayLineWrite(del, write);
	return -write * coefficient + read;
}