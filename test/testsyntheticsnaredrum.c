#include <math.h>
#include <stdio.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

float fixNan(float n){
	return n != n ? 0.f : n;
}

typedef struct {
  Metro tick;
  SyntheticSnareDrum sbd;
} paData;

float freq = 44.f;

static int AudioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  paData *data = (paData *)userData;
  float *out = (float *)outputBuffer;
  (void)inputBuffer; /* Prevent unused variable warning. */

  unsigned int i;
  for (i = 0; i < framesPerBuffer; i++) {
	bool trig = MetroProcess(&data->tick);
	if(trig){
		data->sbd.fm_amount_ = (float)rand() * kRandFrac;
		data->sbd.accent_ = (float)rand() * kRandFrac;
		data->sbd.snappy_ = (float)rand() * kRandFrac;
		data->sbd.decay_ = (float)rand() * kRandFrac;
		data->sbd.f0_ = (float)rand() * kRandFrac * .004 + .002;
	}
	
    float sig = SyntheticSnareDrumProcess(&data->sbd, trig);
    *out++ = fixNan(sig); /* left  */
    *out++ = fixNan(sig); /* right */
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  printf("Marguerite Test: Synth Snare Drum module. \n");

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = 2.f / SAMPLE_RATE;
 
  SyntheticSnareDrumInit(&data.sbd, SAMPLE_RATE);

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