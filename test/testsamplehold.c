#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

typedef struct {
  VariableShapeOsc osc;
  Metro tick;
  SampleHold sh;

  bool sq; //square pulse, will be flipped by metro
} paData;

float mr(){
  return (float)rand() / (RAND_MAX * 60.f);
}

static int AudioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  paData *data = (paData *)userData;
  float *out = (float *)outputBuffer;
  (void)inputBuffer; /* Prevent unused variable warning. */

  unsigned int i;
  for (i = 0; i < framesPerBuffer; i++) {
    bool t = MetroProcess(&data->tick);
    data->sq ^= t;
    
    (data->osc).slave_frequency_ = SampleHoldProcess(&data->sh, data->sq, mr());
    (data->sh).trackOn_ ^= data->sq & t;
    
    float sig = VariableShapeOscProcess(&data->osc);

    *out++ = sig; /* left  */
    *out++ = sig; /* right */
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  printf("Marguerite Test: Metro module. \n");

  // Init the variable shape osc module
  VariableShapeOscInit(SAMPLE_RATE, &data.osc);

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = 6.f / SAMPLE_RATE;

  SampleHoldInit(&data.sh);    
  
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
