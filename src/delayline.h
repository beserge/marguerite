#pragma once
#ifndef MRG_DELAY_H
#define MRG_DELAY_H

#include <stdlib.h>
#include <stdint.h>

/** Simple Delay line.
November 2019
By: shensley
Ported by: beserge
*/
	typedef struct{
		float	frac_;
		size_t	write_ptr_;
		size_t	delay_;
		int		max_size_;
		float*	line_;
	} DelayLine;

	//Initialize the delayline
    void DelayLineInit(DelayLine* del);

	//reset the delayline
    void DelayLineReset(DelayLine* del);

	//Set the delay time in samples
	//fractional is OK
    void DelayLineSetDelay(DelayLine* del, float delay);

	//Write the next sample in
    void DelayLineWrite(DelayLine* del, float sample);

	//Read the next sample
    float DelayLineRead(DelayLine* del);

	//Read from a set location
    float DelayLineReadLoc(DelayLine* del, float delay);

	//Hermite read, from pichenettes
    float DelayLineReadHermite(DelayLine* del, float delay);

	//Allpass implementation, from pichenettes
    float DelayLineAllpass(DelayLine* del, float sample, size_t delay, float coefficient);
	
#endif