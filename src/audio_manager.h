#pragma once

#include <containers.h>
#include <math/vector3.h>

#include <windows.h>
#include <xaudio2.h>

enum class SoundName
{
    Shoot = 0,
    Count
};

struct Sound
{
    IXAudio2SourceVoice* voice;
    int index;
};

struct Wav
{
    WAVEFORMATEXTENSIBLE format;
    void* data;
    size_t size;
};

class AudioManager
{
public:
    static void Init(int memoryType);
    static void Terminate();
    static AudioManager* Get();

    void LoadSound(SoundName sound, const char* filepath);
    void PlaySoundFx(SoundName sound, bool loop);
    void PlaySoundFxAtPosition(SoundName sound, const Vector3& position, bool loop);
    void Update();
    
private:
    void Initialize(int memoryType);
    void Shutdown();

    Sound GetSound();
    void ReleaseSound(Sound sound);

    static AudioManager* instance;

    IXAudio2* xAudio2 = nullptr;
    IXAudio2MasteringVoice* masterVoice = nullptr;
    StaticArray<Wav, static_cast<int>(SoundName::Count)> wavs;

    StaticArray<IXAudio2SourceVoice*, 32> voices;
    StaticArray<int, 32> freeList;
    StaticArray<Sound, 64> playingSounds;

    int firstFree = 0;

};