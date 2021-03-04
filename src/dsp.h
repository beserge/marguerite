#pragma once
#ifndef MRG_DSP_H
#define MRG_DSP_H

#include <stdint.h>
#include <math.h>

#define PI_F 3.1415927410125732421875f
#define TWOPI_F (2.0f * PI_F)

/** quick fp clamp
*/
inline float fclamp(float in, float min, float max)
{
    return fmin(fmax(in, min), max);
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
*/
inline float ThisBlepSample(float t)
{
    return 0.5f * t * t;
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
*/
inline float NextBlepSample(float t)
{
    t = 1.0f - t;
    return -0.5f * t * t;
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
*/
inline float NextIntegratedBlepSample(float t)
{
    const float t1 = 0.5f * t;
    const float t2 = t1 * t1;
    const float t4 = t2 * t2;
    return 0.1875f - t1 + 1.5f * t2 - t4;
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
*/
inline float ThisIntegratedBlepSample(float t)
{
    return NextIntegratedBlepSample(1.0f - t);
}

#endif