#pragma once

class ISceneBuilder;
class GameContext;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	std::string m_settingFile;

public:
	// ¶¬
	void RenderStart();
	// •`‰æ
	void Begin();
	void End();
	// ”jŠü
	void RenderFinalize();
};
