#include "pch.h"
#include "AudioSource.h"
#include "Framework/ECS/GameContext.h"
#include "Framework/Context/ApplicationHandler.h"

void AudioSource::RenderStart()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	if (soundSystem.IsEventPlaying(event))
		soundSystem.StopEvent(event);

	soundSystem.LoadBank(bank, FMOD_STUDIO_LOAD_BANK_NORMAL);
	soundSystem.LoadEvent(event);
	if (GameContext::Get<ApplicationHandler>().IsPlaying())
	{
		if (playOnAwake)
			Play();
	}
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
	{
		std::string tmpname = bank;
		tmpname.resize(128);
		if (ImGui::InputText("Bank Name", &tmpname[0], tmpname.size()))
		{
			bank = std::string(tmpname.c_str());
			RenderStart();
		}
	}

	{
		std::string tmpname = event;
		tmpname.resize(128);
		if (ImGui::InputText("Event Name", &tmpname[0], tmpname.size()))
		{
			event = std::string(tmpname.c_str());
			RenderStart();
		}
	}

	if (ImGui::Checkbox("Play On Awake", &playOnAwake))
		RenderStart();
}
