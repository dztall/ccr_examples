#if !defined(_WIN32)
#error "Not supported platform!!"
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <Objbase.h>
#include <XAudio2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

HRESULT PlayWave(IXAudio2* pXaudio2);

int main()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	IXAudio2* pXAudio2 = NULL;

	UINT32 flags = 0;
	HRESULT hr = XAudio2Create(&pXAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		wprintf(L"Failed to init XAudio2 engine: %#X\n", hr);
		CoUninitialize();
		return 0;
	}

	IXAudio2MasteringVoice* pMasteringVoice = NULL;

	if (FAILED(hr = IXAudio2_CreateMasteringVoice(pXAudio2, &pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, NULL, NULL, AudioCategory_GameEffects)))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		if (pXAudio2)
			IXAudio2_Release(pXAudio2);
		CoUninitialize();
		return 0;
	}

	wprintf(L"Playing mono WAV PCM file...");
	if (FAILED(hr = PlayWave(pXAudio2)))
	{
		wprintf(L"Failed creating source voice: %#X\n", hr);
		if (pXAudio2)
			IXAudio2_Release(pXAudio2);
		CoUninitialize();
		return 0;
	}

	wprintf(L"\nFinished playing\n");

	IXAudio2Voice_DestroyVoice((IXAudio2Voice *)pMasteringVoice);

	if (pXAudio2)
		IXAudio2_Release(pXAudio2);

	CoUninitialize();
}

struct WAVData
{
	WAVEFORMATEX wfx;
	const uint8_t* startAudio;
	uint32_t audioBytes;
};

HRESULT PlayWave(IXAudio2* pXaudio2)
{
	HRESULT hr = S_OK;

	WAVData waveData;
	waveData.wfx.cbSize = 0;
	waveData.wfx.wFormatTag = WAVE_FORMAT_PCM;
	waveData.wfx.nChannels = 1;
	waveData.wfx.nSamplesPerSec = 44100;
	waveData.wfx.nAvgBytesPerSec = 88200;
	waveData.wfx.nBlockAlign = 2;
	waveData.wfx.wBitsPerSample = 16;
	waveData.audioBytes = 44100 * 2;
	waveData.startAudio = (const uint8_t*)malloc(waveData.audioBytes);
	memset(waveData.startAudio, 0, waveData.audioBytes);

	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = IXAudio2_CreateSourceVoice(pXaudio2, &pSourceVoice, &waveData.wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		return hr;
	}

	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = waveData.audioBytes;
	buffer.PlayBegin = 0;
	buffer.PlayLength = 0;
	buffer.LoopBegin = 0;
	buffer.LoopLength = 0;
	buffer.LoopCount = 0;
	buffer.pContext = NULL;
	if (FAILED(hr = IXAudio2SourceVoice_SubmitSourceBuffer(pSourceVoice, &buffer, NULL)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		IXAudio2Voice_DestroyVoice((IXAudio2Voice *)pSourceVoice);
		free(waveData.startAudio);
		return hr;
	}

	free(waveData.startAudio);

	hr = IXAudio2SourceVoice_Start(pSourceVoice, 0, XAUDIO2_COMMIT_NOW);

	bool isRunning = TRUE;
	while (SUCCEEDED(hr) && isRunning)
	{
		XAUDIO2_VOICE_STATE state;
		IXAudio2SourceVoice_GetState(pSourceVoice, &state, 0);
		isRunning = (state.BuffersQueued > 0) != 0;

		if (GetAsyncKeyState(VK_ESCAPE))
			break;

		Sleep(10);
	}

	while (GetAsyncKeyState(VK_ESCAPE))
		Sleep(10);

	IXAudio2Voice_DestroyVoice((IXAudio2Voice *)pSourceVoice);

	return hr;
}
