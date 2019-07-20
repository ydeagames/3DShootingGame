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
	// ê∂ê¨
	void Initialize(GameContext& context);
	// çXêV
	void Update(GameContext& context);
	// ï`âÊ
	void Render(GameContext& context);
	// îjä¸
	void Finalize(GameContext& context);
};
