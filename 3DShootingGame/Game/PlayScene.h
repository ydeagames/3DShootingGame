#pragma once

#include <Framework/ISceneBuilder.h>

class PlayScene : public ISceneBuilder
{
public:
	void Build(GameContext& context) override;
};

