#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class TranslateFollower : public Component
{
public:
	static constexpr const char* Identifier = "TranslateFollower";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// 動くLerpの値
	float movePercent = .5f;
	// 追尾ターゲット
	entt::entity target = entt::null;

public:
	void Update();

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

class RotateFollower : public Component
{
public:
	static constexpr const char* Identifier = "RotateFollower";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// 動くLerpの値
	float movePercent = .5f;
	// 追尾ターゲット
	entt::entity target = entt::null;

public:
	void Update();

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

