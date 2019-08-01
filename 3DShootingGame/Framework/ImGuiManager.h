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
	std::string m_settingFile;

public:
	// ê∂ê¨
	void Initialize(GameContext& context);
	// çXêV
	void Update(GameContext& context);
	// ï`âÊ
	void BeforeRender(GameContext& context);
	void Render(GameContext& context);
	void AfterRender(GameContext& context);
	// îjä¸
	void Finalize(GameContext& context);
};
