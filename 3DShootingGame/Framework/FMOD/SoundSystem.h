/// FMODを扱うライブラリ
#pragma once

// FMOD実装
class SoundSystem
{
private:
	class Implementation;
	std::unique_ptr<Implementation> sgpImplementation;

public:
	SoundSystem();
	~SoundSystem();
	
	void Update();

	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnloadSound(const std::string& strSoundName);
	// void Set3dListenerAndOrientation(const DirectX::SimpleMath::Vector3& vPos = DirectX::SimpleMath::Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	int PlaySounds(const std::string& strSoundName, const DirectX::SimpleMath::Vector3& vPos = DirectX::SimpleMath::Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	void PlayEvent(const std::string& strEventName);
	// void StopChannel(int nChannelId);
	void StopEvent(const std::string& strEventName, bool bImmediate = false);
	void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
	void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
	// void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const DirectX::SimpleMath::Vector3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);
	// bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const std::string& strEventName) const;
	
	static int ErrorCheck(FMOD_RESULT result);
	static FMOD_VECTOR VectorToFmod(const DirectX::SimpleMath::Vector3& vPosition);
	static float DbToVolume(float db);
	static float VolumeToDb(float volume);
};
