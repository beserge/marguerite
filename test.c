#include "callback.c"

void AudioCallback(float** in, float** out, size_t size){
}

int main(){
	StartCallback(48000, 48, AudioCallback);
	
	while(1){}
}