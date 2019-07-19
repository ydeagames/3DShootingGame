#pragma once
#include <Framework/ISceneBuilder.h>

class BuildSettings : public ISceneBuilder
{
public:
	const static std::wstring GAME_TITLE;
	const static std::wstring GAME_TITLE_CLASS;

public:
	void Build(GameContext& context) override;
};

