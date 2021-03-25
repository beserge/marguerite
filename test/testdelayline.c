#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000

//5 seconds
#define DEL_MAX 41000 * 5

float fixNan(float n){
	return n != n ? 0.f : n;
}

typedef struct
{
	VariableShapeOsc osc;
	Metro tick;
	Adsr adsr;
	DelayLine del;
}
paData;

float line[DEL_MAX];

static int AudioCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paData *data = (paData*)userData;
    float *out = (float*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    unsigned int i;
    for(i = 0; i < framesPerBuffer; i++)
    {
		float adsrsig = AdsrProcess(&data->adsr, MetroProcess(&data->tick));	
		
		float oscsig = VariableShapeOscProcess(&data->osc);
		oscsig = fixNan(adsrsig * oscsig);
		
		float delsig = DelayLineRead(&data->del);
		DelayLineWrite(&data->del, (oscsig + delsig) * .5);
		
		*out++ = (delsig + oscsig) * .5f;  /* left  */
        *out++ = (delsig + oscsig) * .5f;  /* right */
    }

    return 0;
}

static paData data;
int main(void)
{
    PaStream *stream;
    PaError err;

    printf("Marguerite Test: Variable Shape Oscillator. \n");

	//Init the variable shape osc module
	VariableShapeOscInit(&data.osc, SAMPLE_RATE);

	//Init the adsr
	AdsrInit(&data.adsr, SAMPLE_RATE);
	data.adsr.seg_time_[ADSR_SEG_ATTACK] = 0.f;

	//Init the metronome
	MetroInit(&data.tick, SAMPLE_RATE);

	//Init the delayline
	data.del.line_ = line;
	data.del.max_size_ = DEL_MAX;
	DelayLineInit(&data.del);
	DelayLineSetDelay(&data.del, SAMPLE_RATE / 4.f);

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                256,        /* frames per buffer */
                                AudioCallback,
                                &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;
	
	//loop forever
	while(1){
	}

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}