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
	std::string bank;
	std::string event;
	bool playOnAwake = true;

public:
	void RenderStart();
	void OnDestroy();

public:
	void Play();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(bank), CEREAL_OPTIONAL_NVP(event), CEREAL_OPTIONAL_NVP(playOnAwake));
	}

	void EditorGui();
};
