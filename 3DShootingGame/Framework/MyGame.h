#pragma once

class GameCamera;

class MyGame
{
private:
	class ImGuiPtr;
	std::unique_ptr<ImGuiPtr> m_imgui;

public:
	MyGame();
	~MyGame();
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);

	int Bench();
};
