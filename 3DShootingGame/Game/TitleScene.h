#pragma once

#include <Framework/ISceneBuilder.h>

class TitleScene : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"TitleScene"; }
	void Build(GameContext& context) override;
};

