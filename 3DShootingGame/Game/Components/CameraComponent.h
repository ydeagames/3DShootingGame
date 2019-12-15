#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class CameraComponent : public Component
{
public:
	static constexpr const char* Identifier = "CameraComponent";

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