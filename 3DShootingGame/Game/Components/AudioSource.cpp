// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-20 07:31:53 +0900
// Modified: 2020-02-07 08:55:28 +0900

#include "pch.h"
#include "AudioSource.h"
#include "Framework/ECS/GameContext.h"
#include "Framework/Context/ApplicationHandler.h"
#include "Utilities/WindowsUtils.h"

void AudioSource::Start()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	if (soundSystem.IsEventPlaying(event))
		soundSystem.StopEvent(event);

	soundSystem.LoadBank(bank, FMOD_STUDIO_LOAD_BANK_NORMAL);
	soundSystem.LoadBank(WindowsUtils::GetDirPath(bank) + WindowsUtils::GetFileName(bank, "bank") + ".strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	soundSystem.LoadEvent(event);
	if (playOnAwake)
		Play();
}

void AudioSource::OnDestroy()
{
	if (!GameContext::Has<SoundSystem>())
		return;

	auto& soundSystem = GameContext::Get<SoundSystem>();
	soundSystem.StopEvent(event);
}

void AudioSource::Play()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	soundSystem.PlayEvent(event);
}

void AudioSource::EditorGui()
{
	bool isPlaying = GameContext::Get<ApplicationHandler>().IsPlaying();

	{
		std::string tmpname = bank;
		tmpname.resize(128);
		if (ImGui::InputText("Bank Name", &tmpname[0], tmpname.size()))
		{
			bank = std::string(tmpname.c_str());
			if (isPlaying)
				Start();
		}
	}

	{
		std::string tmpname = event;
		tmpname.resize(128);
		if (ImGui::InputText("Event Name", &tmpname[0], tmpname.size()))
		{
			event = std::string(tmpname.c_str());
			if (isPlaying)
				Start();
		}
	}

	if (ImGui::Checkbox("Play On Awake", &playOnAwake))
		if (isPlaying)
			Start();
}


