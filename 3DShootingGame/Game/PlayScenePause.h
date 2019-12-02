#pragma once

class PlayScenePause
{
public:
	std::wstring GetName() const { return L"PlayScenePause"; }
	void Build(GameContext& context);
};

