#pragma once
#include <Framework/Component.h>

class GameContext;

class ImGuiManager : public Component
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	std::vector<std::function<void(GameContext&)>> windows;

public:
	void RegisterWindow(const std::function<void(GameContext&)>& window)
	{
		windows.push_back(window);
	}

public:
	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};