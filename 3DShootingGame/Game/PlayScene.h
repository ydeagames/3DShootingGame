#pragma once

#include <Framework/ISceneBuilder.h>

class PlayScene : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"PlayScene"; }
	void Build(GameContext& context) override;
};

