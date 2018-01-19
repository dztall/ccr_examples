//Original source code found at : http://stackoverflow.com/questions/4087727/openal-how-to-create-simple-microphone-echo-programm/5518320#5518320

#if !defined(_OS_IOS_) && !defined(_OS_OSX_)
#error "Not supported platform!!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

void exit_handler();

//C++ std::list<ALuint> workaround.
#define MAX_BUFFER_QUEUE 512
int bufferQueueBegin = 0, bufferQueueEnd = 0;
// A quick and dirty queue of buffer objects
ALuint bufferQueue[MAX_BUFFER_QUEUE];

bool IsQueueEmpty()
{
	return bufferQueueBegin == bufferQueueEnd;
}

void PushQueue(ALuint buffer)
{
	int nextBufferEnd = bufferQueueEnd + 1;
	if (nextBufferEnd >= MAX_BUFFER_QUEUE)
		nextBufferEnd = 0;

	//Prevent cursor collision.
	if (bufferQueueBegin == nextBufferEnd)
		return;

	bufferQueue[bufferQueueEnd] = buffer;
	bufferQueueEnd = nextBufferEnd;
}

ALuint PopFrontQueue()
{
	if (IsQueueEmpty())
		return 0;

	ALuint returnValue = bufferQueue[bufferQueueBegin++];
	if (bufferQueueBegin >= MAX_BUFFER_QUEUE)
		bufferQueueBegin = 0;
	return returnValue;
}

//Main program

#define FREQ 22050   // Sample rate
#define CAP_SIZE 512 // How much to capture at a time (affects latency)
#define NUM_BUFFER 16

ALCdevice *audioDevice = NULL;
ALCcontext *audioContext = NULL;
ALCdevice *inputDevice = NULL;
ALuint helloBuffer[NUM_BUFFER], helloSource[1];
ALenum errorCode = 0;

int main()
{
	//Clang/LLVM on iOS does not support function pointer yet.
#if !CCR_FORCE_LLVM_INTERPRETER
	atexit(exit_handler);
#endif

	// Request default audio device
	audioDevice = alcOpenDevice(NULL);
	errorCode = alcGetError(audioDevice);
	if (errorCode != AL_NO_ERROR)
		printf("1 Error=%d\n", errorCode);
	// Create the audio context
	audioContext = alcCreateContext(audioDevice, NULL);
	alcMakeContextCurrent(audioContext);
	errorCode = alcGetError(audioDevice);
	if (errorCode != AL_NO_ERROR)
		printf("2 Error=%d\n", errorCode);
	// Request the default capture device with a half-second buffer
	inputDevice = alcCaptureOpenDevice(NULL, FREQ, AL_FORMAT_MONO16, FREQ / 2);
	errorCode = alcGetError(inputDevice);
	if (errorCode != AL_NO_ERROR)
		printf("3 Error=%d\n", errorCode);
	// Begin capturing
	alcCaptureStart(inputDevice);
	errorCode = alcGetError(inputDevice);
	if (errorCode != AL_NO_ERROR)
		printf("4 Error=%d\n", errorCode);

	alGenBuffers(NUM_BUFFER, &helloBuffer[0]); // Create some buffer-objects
	errorCode = alGetError();
	if (errorCode != AL_NO_ERROR)
		printf("5 Error=%d\n", errorCode);

	// Queue our buffers onto an STL list
	for (int ii = 0; ii < NUM_BUFFER; ++ii)
		PushQueue(helloBuffer[ii]);

	alGenSources(1, &helloSource[0]); // Create a sound source
	errorCode = alGetError();
	if (errorCode != AL_NO_ERROR)
		printf("6 Error=%d\n", errorCode);

	short buffer[FREQ * 2]; // A buffer to hold captured audio
	ALCint samplesIn = 0;  // How many samples are captured
	ALint availBuffers = 0; // Buffers to be recovered
	ALuint myBuff; // The buffer we're using
	ALuint buffHolder[NUM_BUFFER]; // An array to hold catch the unqueued buffers
	bool done = false;

	// Main loop
	while (!done)
	{
		// Poll for recoverable buffers.
		alGetSourcei(helloSource[0], AL_BUFFERS_PROCESSED, &availBuffers);
		if (availBuffers > 0)
		{
			alSourceUnqueueBuffers(helloSource[0], availBuffers, buffHolder);
			for (int ii = 0; ii < availBuffers; ++ii)
			{
				// Push the recovered buffers back on the queue.
				PushQueue(buffHolder[ii]);
			}
		}
		// Poll for captured audio.
		alcGetIntegerv(inputDevice, ALC_CAPTURE_SAMPLES, 1, &samplesIn);
		if (samplesIn > CAP_SIZE)
		{
			// Grab the sound
			alcCaptureSamples(inputDevice, buffer, CAP_SIZE);

			//***** Process/filter captured data here *****//
			for (int ii = 0; ii < CAP_SIZE; ++ii)
				buffer[ii] *= 4; // Make it louder.

			// Stuff the captured data in a buffer-object.
			if (!IsQueueEmpty())
			{
				// We just drop the data if no buffers are available.
				myBuff = PopFrontQueue();
				alBufferData(myBuff, AL_FORMAT_MONO16, buffer, CAP_SIZE * sizeof(short), FREQ);

				// Queue the buffer
				alSourceQueueBuffers(helloSource[0], 1, &myBuff);

				// Restart the source if needed.
				// (if we take too long and the queue dries up,
				//  the source stops playing).
				ALint sState = 0;
				alGetSourcei(helloSource[0], AL_SOURCE_STATE, &sState);
				if (sState != AL_PLAYING)
					alSourcePlay(helloSource[0]);
			}
		}
	}

#if CCR_FORCE_LLVM_INTERPRETER
	exit_handler();
#endif

	return 0;
}

//Called on normal/abnormal exit.
void exit_handler()
{
	// Stop capture.
	alcCaptureStop(inputDevice);
	alcCaptureCloseDevice(inputDevice);

	// Stop the sources.
	alSourceStopv(1, &helloSource[0]);
	for (int ii = 0; ii < 1; ++ii)
		alSourcei(helloSource[ii], AL_BUFFER, 0);

	// Clean-up.
	alDeleteSources(1, &helloSource[0]);
	alDeleteBuffers(NUM_BUFFER, &helloBuffer[0]);
	errorCode = alGetError();
	if (errorCode != AL_NO_ERROR)
		printf("7 Error=%d\n", errorCode);
	alcMakeContextCurrent(NULL);
	errorCode = alGetError();
	if (errorCode != AL_NO_ERROR)
		printf("8 Error=%d\n", errorCode);
	alcDestroyContext(audioContext);
	alcCloseDevice(audioDevice);
}
