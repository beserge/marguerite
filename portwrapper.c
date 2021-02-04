#include <stdio.h>
#include <math.h>
#include <portaudio.h>

typedef void (*MargueriteCb)(float **in, float **out, size_t size);

static int internalCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    float *out = (float*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    unsigned int i;
    for(i = 0; i < framesPerBuffer; i++)
    {
        *out++ = 0.f;  //output nothing for now
        *out++ = 0.f;
    }

    return 0;
}

static paTestData data;
int Init(int inputChannels, int outputChannels, int framesperbuffer, int samplerate MargueriteCallback cb){
{
    PaStream *stream;
    PaError err;

    /* Initialize library before making any other calls. */
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                inputChannels,          /* no input channels */
                                outputChannels,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                samplerate,
                                framesperbuffer,        /* frames per buffer */
                                internalCallback,
                                &data );
								
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}