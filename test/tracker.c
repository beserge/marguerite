#include <math.h>
#include <stdio.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

#define NUM_TRACKS 4

typedef struct {
	FILE* fp[NUM_TRACKS];
	int linenum;
} paData;

void ParseLine(paData* data){
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

	for(int i = 0; i < 4; i++){
		read = getline(&line, &len, data->fp[i]);
		if(read == -1){
			printf("EOF\n");
			exit(EXIT_SUCCESS);
		}
		printf("%s", line);
	}

    if (line)
        free(line);
}

void OpenTracks(paData* data){
	char path[32];
	for(int i = 0; i < NUM_TRACKS; i++){
		sprintf(path, "./tracks/t%d.txt", i + 1);
		data->fp[i] = fopen(path, "r");	
	}

	//error check
	for(int i = 0; i < NUM_TRACKS; i++){
		if(data->fp[i] == NULL){
			printf("Failed to open track\n");
	        exit(EXIT_FAILURE);
		}
	}
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
    *out++ = 0.f;
    *out++ = 0.f;
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  OpenTracks(&data);
  ParseLine(&data);

  printf("Marguerite Test: Simple Tracker\n");

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