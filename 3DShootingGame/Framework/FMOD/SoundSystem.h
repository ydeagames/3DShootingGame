#pragma once

using Sound = FMOD::Sound*;

class SoundSystem
{
private:
	// Pointer to the FMOD instance
	FMOD::System* m_pSystem;

public:
	SoundSystem();
	~SoundSystem();

public:
	void Create(Sound* pSound, const char* pFile);
	void Play(Sound pSound, bool bLoop = false);
	void Release(Sound pSound);
};
