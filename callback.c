#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define MAX_BLOCKSIZE 128
#define MAX_CHANNELS 4

//non interleaved callback
typedef void (*MargueriteCb)(float **in, float **out, size_t size);
MargueriteCb cb_;

float** in_;
float** out_;
size_t size_;
int channels_;

static void
handler(int sig, siginfo_t *si, void *uc)
{
	cb_(in_, out_, size_);
}

//returns -1 on failure, 0 otherwise
//cb rate in Hz.
int StartCallback(long long callback_rate, int blocksize, int channels, MargueriteCb cb){
	cb_ = cb;

	size_ = blocksize;
	channels_ = channels;
	
	in_ = malloc(MAX_BLOCKSIZE * MAX_CHANNELS * sizeof(float));
	out_ = malloc(MAX_BLOCKSIZE * MAX_CHANNELS * sizeof(float));
	
	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;
	long long freq_nanosecs;
	sigset_t mask;
	struct sigaction sa;
	
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIG, &sa, NULL);
	
	sigemptyset(&mask);
	sigaddset(&mask, SIG);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	
	sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCKID, &sev, &timerid);
		 
	freq_nanosecs = 1000000000 / callback_rate;
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
	
	timer_settime(timerid, 0, &its, NULL);
	
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	
	return 0;
}
