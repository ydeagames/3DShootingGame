// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-23 07:07:20 +0900
// Modified: 2020-02-07 10:59:30 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class Goal : public Component
{
public:
	static constexpr const char* Identifier = "Goal";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
		component.DependsOn<Rigidbody>();
	}

public:
	// 追尾ターゲット
	entt::entity target = entt::null;

private:
	bool m_goaled = false;

public:
	void Start();
	void Update();
	void OnDestroy();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(target));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(target);
	}

	void EditorGui();
};

class Death : public Component
{
public:
	static constexpr const char* Identifier = "Death";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
		component.DependsOn<Rigidbody>();
	}

public:
	// 追尾ターゲット
	entt::entity target = entt::null;

private:
	bool m_goaled = false;

public:
	void Start();
	void Update();
	void OnDestroy();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(target));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(target);
	}

	void EditorGui();
};


