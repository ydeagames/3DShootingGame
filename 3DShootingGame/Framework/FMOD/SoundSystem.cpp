#include "pch.h"
#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
	if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
	{
		// Report Error
		throw std::exception("Sound System is not created");
	}

	int driverCount = 0;
	m_pSystem->getNumDrivers(&driverCount);

	if (driverCount == 0)
	{
		// Report Error
		throw std::exception("Sound Driver is not available");
	}

	// Initialize our Instance with 36 Channels
	m_pSystem->init(36, FMOD_INIT_NORMAL, NULL);
}

SoundSystem::~SoundSystem()
{
	// if (m_pSystem->close() == 0)
	// {
	// 	// Report Error
	// 	throw std::exception("Sound System is not closed");
	// }
	//
	// if (m_pSystem->release() == 0)
	// {
	// 	// Report Error
	// 	throw std::exception("Sound System is not released");
	// }
}

void SoundSystem::Create(Sound* pSound, const char* pFile)
{
	m_pSystem->createSound(pFile, FMOD_3D, 0, pSound);
}

void SoundSystem::Play(Sound pSound, bool bLoop)
{
	if (!bLoop)
		pSound->setMode(FMOD_LOOP_OFF);
	else
	{
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	m_pSystem->playSound(pSound, nullptr, false, 0);
}

void SoundSystem::Release(Sound pSound)
{
	pSound->release();
}
