#if !defined(_OS_IOS_) && !defined(_OS_OSX_)
#error "Not supported platform!!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

void exit_handler();

static ALCdevice *openALDevice = NULL;
static ALCcontext *openALContext = NULL;
ALuint bufferID = 0;
unsigned int sourceID = 0;

int main()
{
	//Clang/LLVM on iOS does not support function pointer yet.
#if !CCR_FORCE_LLVM_INTERPRETER
	atexit(exit_handler);
#endif

	//Create context.
	openALDevice = alcOpenDevice(NULL);
	openALContext = alcCreateContext(openALDevice, NULL);
	alcMakeContextCurrent(openALContext);

	//Fill Buffer.
	const int audioSampleSize = 44100;
	short *audioData = (short *)malloc(audioSampleSize * sizeof(short));

	for (int i = 0; i < audioSampleSize; i++)
		audioData[i] = 25000.0f*sinf(i / 10.0f);

	alGenBuffers(1, &bufferID);
	alBufferData(bufferID, AL_FORMAT_MONO16, audioData, audioSampleSize * sizeof(short), 44100);
	free(audioData);

	//Play.
	alGenSources(1, &sourceID);
	alSourcef(sourceID, AL_PITCH, 1.0f);
	alSourcef(sourceID, AL_GAIN, 1.0f);
	alSourcei(sourceID, AL_BUFFER, bufferID);
	alSourcePlay(sourceID);

	usleep(1000 * 1000);

#if CCR_FORCE_LLVM_INTERPRETER
	exit_handler();
#endif

	return 0;
}

//Called on normal/abnormal exit.
void exit_handler()
{
	//Destroy resources.
	if (sourceID)
	{
		alSourceStop(sourceID);
		alDeleteSources(1, &sourceID);
		sourceID = 0;
	}

	if (bufferID)
	{
		alDeleteBuffers(1, &bufferID);
		bufferID = 0;
	}

	//Finalize context.
	alcMakeContextCurrent(NULL);
	if (openALContext)
	{
		alcDestroyContext(openALContext);
		openALContext = NULL;
	}

	if (openALDevice)
	{
		alcCloseDevice(openALDevice);
		openALDevice = NULL;
	}
}
