// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-16 22:30:22 +0900
// Modified: 2019-12-23 05:07:51 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class AbstractFollower : public Component
{
public:
	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// ฎญLerpฬl
	float movePercent = .5f;
	// ว๖^[Qbg
	entt::entity target = entt::null;

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(movePercent), CEREAL_OPTIONAL_NVP(target));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(target);
	}

	void EditorGui();
};

class TranslateFollower : public AbstractFollower
{
public:
	static constexpr const char* Identifier = "TranslateFollower";

public:
	void Update();
};

class RotateFollower : public AbstractFollower
{
public:
	static constexpr const char* Identifier = "RotateFollower";

public:
	void Update();
};

class TrackingFollower : public AbstractFollower
{
public:
	static constexpr const char* Identifier = "TrackingFollower";

	// ผa
	float radius = 5;
	// ณ
	float height = 4;

public:
	void Update();

	template<class Archive>
	void serialize(Archive& archive)
	{
		AbstractFollower::serialize(archive);
		archive(CEREAL_OPTIONAL_NVP(radius), CEREAL_OPTIONAL_NVP(height));
	}

	void EditorGui();
};


