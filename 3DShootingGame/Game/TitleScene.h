#pragma once

#include <Framework/ISceneBuilder.h>

class TitleScene : public ISceneBuilder
{
public:
	void Build(GameContext& context) override;
};

