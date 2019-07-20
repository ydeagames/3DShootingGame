#pragma once

#include <Framework/ISceneBuilder.h>

class PlayScenePause : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"PlayScenePause"; }
	void Build(GameContext& context) override;
};

