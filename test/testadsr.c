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
  VariableShapeOsc osc;
  Metro tick;
  Adsr adsr;
  bool keypress;

  float pos;
  float seq[5];
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
      data->pos = (data->pos + .5f);
      data->pos = data->pos >= 5.f ? 0.f : data->pos;
	  
      data->osc.slave_frequency_ = data->seq[(int)data->pos];
	  
	  data->keypress ^= true; //flip the keypress state
    }

	float adsrsig = AdsrProcess(&data->adsr, data->keypress);
	data->osc.waveshape_ = adsrsig;

    float sig = VariableShapeOscProcess(&data->osc);
	sig *= adsrsig;

    *out++ = fixNan(sig * adsrsig); /* left  */
    *out++ = fixNan(sig * adsrsig); /* right */
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  printf("Marguerite Test: Adsr module. \n");

  // Init the variable shape osc module
  VariableShapeOscInit(&data.osc, SAMPLE_RATE);

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = 1.f / SAMPLE_RATE;

  AdsrInit(&data.adsr, SAMPLE_RATE);
  data.keypress = true;
	

  data.pos = 0;

  data.seq[0] = 220.f / SAMPLE_RATE;
  data.seq[1] = 261.625f / SAMPLE_RATE;
  data.seq[2] = 329.625f / SAMPLE_RATE;
  data.seq[3] = 392.f / SAMPLE_RATE;
  data.seq[4] = 440.f / SAMPLE_RATE;

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