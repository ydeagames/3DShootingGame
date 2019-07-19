#pragma once
#include <Framework/ISceneBuilder.h>

// ゲームロジック
class BaseScene : public ISceneBuilder
{
public:
	// 生成
	void Build(GameContext& context);
};

