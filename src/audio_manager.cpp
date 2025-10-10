#include "audio_manager.h"
#include <memory_manager.h>
#include <new>

// Singleton
AudioManager* AudioManager::instance = nullptr;

void AudioManager::Init(int memoryType)
{
    void* buffer = MemoryManager::Get()->Alloc(sizeof(AudioManager), memoryType);
    memset(buffer, 0, sizeof(AudioManager));
    instance = new (buffer) AudioManager;
    ASSERT(instance != nullptr);
    instance->Initialize(memoryType);
}

void AudioManager::Terminate()
{
    if (instance == nullptr)
    {
        ASSERT(!"Error: audio manager has not been initialize");
    }
    instance->Shutdown();
}

AudioManager* AudioManager::Get()
{
    return instance;
}

HRESULT LoadAudioFile(const char* path, Wav* soundData);

// Class Implementation
void AudioManager::Initialize(int memoryType)
{
    if (FAILED(XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    {
        ASSERT(!"Error: xAudio2 failed to initialize");
    }
    if (FAILED(xAudio2->CreateMasteringVoice(&masterVoice)))
    {
        ASSERT(!"Error: mastering voice failed to create");
    }

    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 44100;
    format.wBitsPerSample = 16;
    format.nBlockAlign = (format.nChannels * format.wBitsPerSample) >> 3;
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    for (int i = 0; i < voices.GetCapacity(); i++)
    {
        IXAudio2SourceVoice* voice = nullptr;
        if (FAILED(xAudio2->CreateSourceVoice(&voice, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, nullptr, nullptr)))
        {
            ASSERT(!"Error: mastering voice failed to create");
        }
        voices.Push(voice);
    }

    // Initialize free list
    firstFree = 0;
    for (int i = 0; i < freeList.GetCapacity(); i++)
    {
        freeList.Push(i + 1);
    }
    freeList[freeList.GetCapacity() - 1] = -1;

    wavs.size = static_cast<int>(SoundName::Count);
}

void AudioManager::Shutdown()
{
    if (masterVoice)
    {
        masterVoice->DestroyVoice();
    }

    for (int i = 0; i < voices.size; i++)
    {
        if (voices[i])
        {
            voices[i]->DestroyVoice();
        }
    }

    if (xAudio2)
    {
        xAudio2->Release();
    }
}

void AudioManager::LoadSound(SoundName sound, const char* filepath)
{
    LoadAudioFile(filepath, &wavs[static_cast<int>(sound)]);
}

void AudioManager::PlaySoundFx(SoundName soundName, bool loop)
{
    Sound sound = GetSound(soundName);
    if (!sound.voice)
    {
        return;
    }

    sound.voice->FlushSourceBuffers();

    // fill the source voice
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = wavs[static_cast<int>(soundName)].size;
    buffer.pAudioData = (BYTE*)wavs[static_cast<int>(soundName)].data;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    
    HRESULT hr = sound.voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
        ASSERT(!"Error: filling voice source");
    }

    sound.voice->Start();
}

void AudioManager::PlaySoundFxAtPosition(SoundName soundName, const Vector3& position, bool loop)
{
    // TODO: ...
}

void AudioManager::StopSoundFx(SoundName soundName)
{
    for (int i = playingSounds.size - 1; i >= 0; i--)
    {
        Sound& sound = playingSounds[i];
        XAUDIO2_VOICE_STATE state;
        if(sound.soundName == soundName)
        {
            sound.voice->Stop();
            ReleaseSound(sound);
            playingSounds[i] = playingSounds[playingSounds.size - 1];
            playingSounds.size--;
        }
    }
}

void AudioManager::Update()
{
    for (int i = playingSounds.size - 1; i >= 0; i--)
    {
        Sound& sound = playingSounds[i];
        XAUDIO2_VOICE_STATE state;
        sound.voice->GetState(&state);
        if (state.BuffersQueued == 0)
        {
            sound.voice->Stop();
            ReleaseSound(sound);
            playingSounds[i] = playingSounds[playingSounds.size - 1];
            playingSounds.size--;
        }
    }
}

Sound AudioManager::GetSound(SoundName soundName)
{
    Sound sound = {};
    if (firstFree > -1)
    {
        sound.voice = voices[firstFree];
        sound.index = firstFree;
        sound.soundName = soundName;
        int nextFree = freeList[firstFree];
        freeList[firstFree] = -1;
        firstFree = nextFree;

        playingSounds.Push(sound);

    }
    return sound;
}

void AudioManager::ReleaseSound(Sound sound)
{
    freeList[sound.index] = firstFree;
    firstFree = sound.index;
    sound.index = -1;
}


// Read Wav files

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK) {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType) {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc) {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;

}

static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

static HRESULT LoadAudioFile(const char* path, Wav* soundData) {

    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return HRESULT_FROM_WIN32(GetLastError());

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    // NOTE: Locate the RIFF chuck
    FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
        return S_FALSE;

    // NOTE: Locate the FMT chuck
    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, &soundData->format, dwChunkSize, dwChunkPosition);

    // NOTE: Locate the DATA chuck
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = (BYTE*)MemoryManager::Get()->Alloc(dwChunkSize, STATIC_MEMORY);
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    // NOTE: Populate XAudio2 buffer
    soundData->size = dwChunkSize;
    soundData->data = (void*)pDataBuffer;

    CloseHandle(hFile);

    return S_OK;
}