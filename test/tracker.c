#include <math.h>
#include <stdio.h>
#include <string.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000
#define NUM_TRACKS 3
#define NUM_ENVS 4

#define DEBUG 1

float fixNan(float num){
	return num != num ? 0.f : num;
}

typedef struct {
	FILE* fp[NUM_TRACKS];
	int linenum;
	int linedata[NUM_TRACKS][3]; //note, comnum, comval

	Oscillator osc1;
	Oscillator osc2;
	VariableShapeOsc vsosc;
	SyntheticBassDrum bd;
	SyntheticSnareDrum sd;

	Adsr envs[NUM_ENVS]; //3 oscs and 1 noise

	Metro tick;
} paData;

void ParseLines(paData* data){
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

	for(int i = 0; i < NUM_TRACKS; i++){
		read = getline(&line, &len, data->fp[i]);
		if(read == -1){
			printf("EOF\n");
			exit(EXIT_SUCCESS);
		}
			
		char * token = strtok(line, " ");
		int j = 0;
		while(token != NULL && j < 3) {
			char* pEnd;
			data->linedata[i][j] = atoi(token);
			token = strtok(NULL, " ");
			j++;
		}
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


//instruments are osc, osc, varishape, kick, snare, env noise
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
	if(t){
		ParseLines(data);
	}
	
	//freqs
	data->osc1.phase_inc_ = mtor(data->linedata[0][0], SAMPLE_RATE);
	data->osc2.phase_inc_ = mtor(data->linedata[1][0], SAMPLE_RATE);
	data->vsosc.slave_frequency_ = mtor(data->linedata[2][0], SAMPLE_RATE);
		
	//process
	float sig = 0.f;
	sig += fixNan(OscillatorProcess(&data->osc1) * AdsrProcess(&data->envs[0], t));
	sig += fixNan(OscillatorProcess(&data->osc2) * AdsrProcess(&data->envs[1], t));
	sig += fixNan(VariableShapeOscProcess(&data->vsosc) * AdsrProcess(&data->envs[2], t));
	sig /= (float)NUM_TRACKS;
	
    *out++ = sig;
    *out++ = sig;
  }

  return 0;
}

static paData data;
int main(void) {
  PaStream *stream;
  PaError err;

  OpenTracks(&data);

  MetroInit(&data.tick, SAMPLE_RATE);
  data.tick.phs_inc_ = ftor(3.f, SAMPLE_RATE);
  
  OscillatorInit(&data.osc1, SAMPLE_RATE);
  OscillatorInit(&data.osc2, SAMPLE_RATE);
  VariableShapeOscInit(&data.vsosc, SAMPLE_RATE);
  SyntheticBassDrumInit(&data.bd, SAMPLE_RATE);
  SyntheticSnareDrumInit(&data.sd, SAMPLE_RATE);

	for(int i = 0; i < NUM_ENVS; i++){
		AdsrInit(&data.envs[i], SAMPLE_RATE);
		data.envs[i].seg_time_[ADSR_SEG_ATTACK] = .00001f;
		data.envs[i].seg_time_[ADSR_SEG_DECAY] = .1f;
	}

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