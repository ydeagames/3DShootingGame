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
	// 動くLerpの値
	float movePercent = .5f;
	// 追尾ターゲット
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

	// 半径
	float radius = 5;
	// 追尾ターゲット
	entt::entity rotateObject = entt::null;

public:
	void Update();

	template<class Archive>
	void serialize(Archive& archive)
	{
		AbstractFollower::serialize(archive);
		archive(CEREAL_OPTIONAL_NVP(radius), CEREAL_OPTIONAL_NVP(rotateObject));
	}

	void EditorGui();
};
