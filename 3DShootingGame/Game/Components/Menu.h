#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class Menu : public Component
{
public:
	static constexpr const char* Identifier = "Menu";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	bool showCredit = false;

public:
	void Update();
	void RenderGui(GameCamera& camera);
};

