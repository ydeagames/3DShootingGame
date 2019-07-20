#pragma once

// 共通インクルード
#include "GameContext.h"
#include "GameObject.h"
#include "GameCamera.h"

class ISceneBuilder
{
public:
	virtual std::wstring GetName() const = 0;
	virtual ~ISceneBuilder() = default;
	virtual void Build(GameContext& context) = 0;
};

