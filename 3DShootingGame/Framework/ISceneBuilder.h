#pragma once

// 共通インクルード
#include "GameContext.h"
#include "GameObject.h"
#include "GameCamera.h"

class ISceneBuilder : public Object
{
public:
	std::wstring GetType() const override { return L"SceneBuilder"; };
	virtual ~ISceneBuilder() = default;
	virtual void Build(GameContext& context) = 0;
};

