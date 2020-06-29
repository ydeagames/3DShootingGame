// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 06:40:16 +0900
// Modified: 2020-02-05 23:03:19 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class LightComponent : public Component
{
public:
	static constexpr const char* Identifier = "LightComponent";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	void OnPreRender(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

