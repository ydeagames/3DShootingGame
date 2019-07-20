#pragma once
#include <Framework/Component.h>

class ISceneBuilder;
class GameContext;

class ImGuiManager : public Component
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	std::vector<std::shared_ptr<ISceneBuilder>> m_windows;
	std::string m_settingFile;

public:
	void Add(const std::shared_ptr<ISceneBuilder>& window)
	{
		m_windows.push_back(window);
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
