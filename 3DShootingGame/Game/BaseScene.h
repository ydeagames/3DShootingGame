#pragma once
#include <Framework/ISceneBuilder.h>

// ゲームロジック
class BaseScene : public ISceneBuilder
{
public:
	std::wstring GetName() const override { return L"BaseScene"; }
	// 生成
	void Build(GameContext& context);
};

