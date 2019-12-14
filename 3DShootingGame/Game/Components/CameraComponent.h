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

protected:
	GameCamera* cameraptr = nullptr;

public:
	void Render(GameCamera& camera);
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};