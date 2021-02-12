#include <stdio.h>
#include <math.h>

#include <portaudio.h>
#include <marguerite.h>

#define SAMPLE_RATE 41000
#define NUM_SECONDS 4

typedef struct
{
	RampOsc osc;
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
        float sig = RampOscProcess(&data->osc);
		*out++ = sig;  /* left  */
        *out++ = sig;  /* right */
    }

    return 0;
}

static paData data;
int main(void)
{
    PaStream *stream;
    PaError err;

    printf("Marguerite Test: Ramp Oscillator. \n");

	//Init the ramp osc module
	RampOscInit(SAMPLE_RATE, &data.osc);

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
	
    /* Sleep for several seconds. */
    Pa_Sleep(NUM_SECONDS*1000);

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    Pa_Terminate();
    printf("Test finished.\n");
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}