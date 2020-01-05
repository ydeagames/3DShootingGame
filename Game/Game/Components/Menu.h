#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class TitleMenu : public Component
{
public:
	static constexpr const char* Identifier = "TitleMenu";

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

class PlayMenu : public Component
{
public:
	static constexpr const char* Identifier = "PlayMenu";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	float last;

public:
	void Start();
	void Update();
	void RenderGui(GameCamera& camera);
};

class ResultMenu : public Component
{
public:
	static constexpr const char* Identifier = "ResultMenu";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	void Update();
	void RenderGui(GameCamera& camera);
};

class PauseMenu : public Component
{
public:
	static constexpr const char* Identifier = "PauseMenu";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	void Update();
	void RenderGui(GameCamera& camera);
};

