#pragma once

#include <Framework/ISceneBuilder.h>

class LogoScene : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"LogoScene"; }
	void Build(GameContext& context) override;
};

