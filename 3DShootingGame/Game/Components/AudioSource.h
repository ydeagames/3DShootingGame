// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-20 07:31:53 +0900
// Modified: 2020-02-07 08:55:28 +0900

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
	void Start();
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


