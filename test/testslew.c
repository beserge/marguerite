#include <math.h>
#include <stdio.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

float fixNan(float n){
	return n != n ? 0.f : n;
}

typedef struct {
  VariableShapeOsc osc;
  Metro tick;
  Slew slew;
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
    if (MetroProcess(&data->tick)) {
		freq *= 2.f;
		if(freq > 800.f){
			freq = 44.f;
		}
	}

	data->osc.slave_frequency_ = SlewProcess(&data->slew, freq) / SAMPLE_RATE;

    float sig = VariableShapeOscProcess(&data->osc);

    *out++ = fixNan(sig); /* left  */
    *out++ = fixNan(sig); /* right */
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  printf("Marguerite Test: Slew module. \n");

  // Init the variable shape osc module
  VariableShapeOscInit(SAMPLE_RATE, &data.osc);

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = 1.f / SAMPLE_RATE;

  SlewInit(&data.slew, SAMPLE_RATE);
  SlewSetRate(&data.slew, .1f);

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