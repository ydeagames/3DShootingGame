#pragma once
#include <Framework/Components/AllComponents.h>
#include <Framework/ECS/Scene.h>

class MyGame
{
private:
	Scene m_scene;
	class ImGuiPtr;
	std::unique_ptr<ImGuiPtr> m_imgui;

public:
	MyGame();
	~MyGame();
	void Update();
	void RenderInitialize();
	void Render(GameCamera& camera);

	int Bench();
};
