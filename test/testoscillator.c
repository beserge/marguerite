#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

float fixNan(float n){
	return n != n ? 0.f : n;
}

typedef struct {
  Oscillator osc;
  Metro tick;
  
  int pos;
  float seq[8];
} paData;

static int AudioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  paData *data = (paData *)userData;
  float *out = (float *)outputBuffer;
  (void)inputBuffer; /* Prevent unused variable warning. */

  unsigned int i;
  for (i = 0; i < framesPerBuffer; i++) {
    if (MetroProcess(&data->tick)) {
      data->pos++;
      data->pos %= 8;
	  
      data->osc.phase_inc_ = data->seq[data->pos];	  
	  
	  if (data->pos == 0){	
		data->osc.waveform_++;
		data->osc.waveform_ %= 5;
	  }
	}

    float sig = OscillatorProcess(&data->osc);

    *out++ = sig; /* left  */
    *out++ = sig; /* right */
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  printf("Marguerite Test: Oscillator module. \n");

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = 12.f / SAMPLE_RATE;	

  // Init the variable shape osc module
  OscillatorInit(&data.osc, SAMPLE_RATE);

  data.pos = 0;
  data.seq[0] = OscillatorCalcPhaseInc(&data.osc, 329.63f);
  data.seq[1] = OscillatorCalcPhaseInc(&data.osc, 392.f);
  data.seq[2] = OscillatorCalcPhaseInc(&data.osc, 440.f);
  data.seq[3] = OscillatorCalcPhaseInc(&data.osc, 392.f);
  data.seq[4] = OscillatorCalcPhaseInc(&data.osc, 587.33f);
  data.seq[5] = OscillatorCalcPhaseInc(&data.osc, 523.25f);
  data.seq[6] = OscillatorCalcPhaseInc(&data.osc, 587.33f);
  data.seq[7] = OscillatorCalcPhaseInc(&data.osc, 659.25);
  
  err = Pa_Initialize();
  if (err != paNoError)
    goto error;

  /* Open an audio I/O stream. */
  err = Pa_OpenDefaultStream(&stream, 0, /* no input channels */
                             2,          /* stereo output */
                             paFloat32,  /* 32 bit floating point output */
                             SAMPLE_RATE, 256, /* frames per buffer */
                             AudioCallback, &data);
  if (err != paNoError)
    goto error;

  err = Pa_StartStream(stream);
  if (err != paNoError)
    goto error;

  // loop forever
  while (1) {
  }

  return err;
error:
  Pa_Terminate();
  fprintf(stderr, "An error occurred while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
  return err;
}