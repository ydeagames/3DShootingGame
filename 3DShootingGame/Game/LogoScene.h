#pragma once

#include <Framework/ISceneBuilder.h>

class LogoScene : public ISceneBuilder
{
public:
	void Build(GameContext& context) override;
};

