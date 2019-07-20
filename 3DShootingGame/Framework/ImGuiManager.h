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
	void RegisterWindow(const std::shared_ptr<ISceneBuilder>& window)
	{
		m_windows.push_back(window);
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
