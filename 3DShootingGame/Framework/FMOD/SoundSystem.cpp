// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-20 07:31:53 +0900
// Modified: 2020-02-07 06:07:13 +0900

#include "pch.h"
#include "SoundSystem.h"

class SoundSystem::Implementation
{
public:
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	using SoundMap = std::unordered_map<std::string, FMOD::Sound*>;
	using ChannelMap = std::unordered_map<int, FMOD::Channel*>;
	using EventMap = std::unordered_map<std::string, FMOD::Studio::EventInstance*>;
	using BankMap = std::unordered_map<std::string, FMOD::Studio::Bank*>;

	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

SoundSystem::Implementation::Implementation()
{
	mpStudioSystem = nullptr;
	SoundSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	SoundSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr));

	mpSystem = nullptr;
	SoundSystem::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

SoundSystem::Implementation::~Implementation()
{
	SoundSystem::ErrorCheck(mpStudioSystem->unloadAll());
	SoundSystem::ErrorCheck(mpStudioSystem->release());
}

void SoundSystem::Implementation::Update()
{
	std::vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			pStoppedChannels.push_back(it);
		}
	}
	for (auto& it : pStoppedChannels)
	{
		mChannels.erase(it);
	}
	SoundSystem::ErrorCheck(mpStudioSystem->update());
}

SoundSystem::SoundSystem()
{
	sgpImplementation = std::make_unique<Implementation>();
}

SoundSystem::~SoundSystem()
{
}

void SoundSystem::Update()
{
	sgpImplementation->Update();
}

void SoundSystem::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
	if (tFoundIt != sgpImplementation->mSounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	SoundSystem::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if (pSound) {
		sgpImplementation->mSounds[strSoundName] = pSound;
	}
}

void SoundSystem::UnloadSound(const std::string& strSoundName)
{
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
	if (tFoundIt == sgpImplementation->mSounds.end())
		return;

	SoundSystem::ErrorCheck(tFoundIt->second->release());
	sgpImplementation->mSounds.erase(tFoundIt);
}

int SoundSystem::PlaySounds(const std::string& strSoundName, const DirectX::SimpleMath::Vector3& vPosition, float fVolumedB)
{
	int nChannelId = sgpImplementation->mnNextChannelId++;
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
	if (tFoundIt == sgpImplementation->mSounds.end())
	{
		LoadSound(strSoundName);
		tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
		{
			return nChannelId;
		}
	}
	FMOD::Channel* pChannel = nullptr;
	SoundSystem::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFmod(vPosition);
			SoundSystem::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}
		SoundSystem::ErrorCheck(pChannel->setVolume(DbToVolume(fVolumedB)));
		SoundSystem::ErrorCheck(pChannel->setPaused(false));
		sgpImplementation->mChannels[nChannelId] = pChannel;
	}
	return nChannelId;
}

void SoundSystem::SetChannel3dPosition(int nChannelId, const DirectX::SimpleMath::Vector3& vPosition)
{
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFmod(vPosition);
	SoundSystem::ErrorCheck(tFoundIt->second->set3DAttributes(&position, nullptr));
}

void SoundSystem::SetChannelVolume(int nChannelId, float fVolumedB)
{
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	SoundSystem::ErrorCheck(tFoundIt->second->setVolume(DbToVolume(fVolumedB)));
}

void SoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
	if (tFoundIt != sgpImplementation->mBanks.end())
		return;
	FMOD::Studio::Bank* pBank;
	SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
	if (pBank) {
		sgpImplementation->mBanks[strBankName] = pBank;
	}
}

void SoundSystem::LoadEvent(const std::string& strEventName)
{
	auto tFoundit = sgpImplementation->mEvents.find(strEventName);
	if (tFoundit != sgpImplementation->mEvents.end())
		return;
	FMOD::Studio::EventDescription* pEventDescription = nullptr;
	SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
	if (pEventDescription) {
		FMOD::Studio::EventInstance* pEventInstance = nullptr;
		SoundSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance) {
			sgpImplementation->mEvents[strEventName] = pEventInstance;
		}
	}
}

void SoundSystem::PlayEvent(const std::string& strEventName)
{
	auto tFoundit = sgpImplementation->mEvents.find(strEventName);
	if (tFoundit == sgpImplementation->mEvents.end()) {
		LoadEvent(strEventName);
		tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit == sgpImplementation->mEvents.end())
			return;
	}
	tFoundit->second->start();
}

void SoundSystem::StopEvent(const std::string& strEventName, bool bImmediate)
{
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	SoundSystem::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool SoundSystem::IsEventPlaying(const std::string& strEventName) const
{
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return true;
	}
	return false;
}

void SoundSystem::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter)
{
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	SoundSystem::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter));
}

void SoundSystem::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue)
{
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	SoundSystem::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}

FMOD_VECTOR SoundSystem::VectorToFmod(const DirectX::SimpleMath::Vector3& vPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

float  SoundSystem::DbToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}

float  SoundSystem::VolumeToDb(float volume)
{
	return 20.0f * log10f(volume);
}

int SoundSystem::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		std::cout << "FMOD ERROR " << result << std::endl;
		return 1;
	}
	// cout << "FMOD all good" << endl;
	return 0;
}


