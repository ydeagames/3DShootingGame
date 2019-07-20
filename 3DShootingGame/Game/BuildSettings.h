#pragma once
#include <Framework/ISceneBuilder.h>

class BuildSettings : public ISceneBuilder
{
public:
	const static std::wstring GAME_TITLE;
	const static std::wstring GAME_TITLE_CLASS;
	std::wstring GetName() const override { return L"BuildSettings"; }

public:
	void Build(GameContext& context) override;
};

