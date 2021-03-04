#include <stdio.h>
#include <math.h>

#include <portaudio.h>
#include <marguerite.h>


#define SAMPLE_RATE 41000

typedef struct
{
	VariableShapeOsc osc;
	RampOsc lfo;
	Svf svf;
}
paData;

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
        float sig = VariableShapeOscProcess(&data->osc);
		SvfProcess(&data->svf, sig);
		float lfo = RampOscProcess(&data->lfo);
		SvfSetFreq(&data->svf, fabsf(lfo) * 5000.f);
								
		*out++ = data->svf.out_low_;  /* left  */
        *out++ = data->svf.out_high_;  /* right */
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
	VariableShapeOscInit(SAMPLE_RATE, &data.osc);

	//Init the ramp lfo
	RampOscInit(SAMPLE_RATE, &data.lfo);
	data.lfo.phase_inc_ = 1.f / (float)SAMPLE_RATE; //freq = 1 Hz

	//Init svf
	SvfInit(&data.svf, SAMPLE_RATE);

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