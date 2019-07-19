#pragma once

#include <Framework/ISceneBuilder.h>

class ResultScene : public ISceneBuilder
{
public:
	void Build(GameContext& context) override;
};

