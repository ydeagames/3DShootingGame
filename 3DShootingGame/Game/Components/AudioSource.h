#pragma once
#include <Framework/ECS/Component.h>
#include <Framework/FMOD/SoundSystem.h>

class AudioSource : public Component
{
public:
	static constexpr const char* Identifier = "AudioSource";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string audioClip;
	bool playOnAwake = true;
	bool loop = true;

private:
	Sound m_sound;

public:
	void RenderStart();
	void OnDestroy();

public:
	void Play();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(audioClip), CEREAL_OPTIONAL_NVP(playOnAwake), CEREAL_OPTIONAL_NVP(loop));
	}

	void EditorGui();
};
