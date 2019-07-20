#pragma once

#include <Framework/ISceneBuilder.h>

class ResultScene : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"ResultScene"; }
	void Build(GameContext& context) override;
};

