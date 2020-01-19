#include "pch.h"
#include "AudioSource.h"
#include "Framework/ECS/GameContext.h"
#include "Framework/Context/ApplicationHandler.h"

void AudioSource::RenderStart()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	if (m_sound)
	{
		soundSystem.Release(m_sound);
		m_sound = nullptr;
	}

	if (GameContext::Get<ApplicationHandler>().IsPlaying())
	{
		soundSystem.Create(&m_sound, audioClip.c_str());
		if (playOnAwake)
			Play();
	}
}

void AudioSource::OnDestroy()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	if (m_sound)
	{
		soundSystem.Release(m_sound);
		m_sound = nullptr;
	}
}

void AudioSource::Play()
{
	auto& soundSystem = GameContext::Get<SoundSystem>();
	if (m_sound)
	{
		soundSystem.Play(m_sound, loop);
	}
}

void AudioSource::EditorGui()
{
	std::string tmpname = audioClip;
	tmpname.resize(128);
	if (ImGui::InputText("Texture", &tmpname[0], tmpname.size()))
	{
		audioClip = std::string(tmpname.c_str());
		RenderStart();
	}

	if (ImGui::Checkbox("Play On Awake", &playOnAwake))
		RenderStart();
	if (ImGui::Checkbox("Loop", &loop))
		RenderStart();
}
