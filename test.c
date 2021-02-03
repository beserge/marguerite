#include "callback.c"

void AudioCallback(float** in, float** out, size_t size){
	for(int i = 0; i < size; i++){
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(){
	StartCallback(48000, 48, 2, AudioCallback); //48kHz, 48 sample block, stereo
	
	while(1){}
}