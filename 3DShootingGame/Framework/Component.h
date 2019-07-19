#pragma once

class GameContext;
class GameObject;

#define COMPONENT(TYPE) \
	public: \
		using Base = TYPE;

// コンポーネント
class Component
{
public:
	Component() = default;
	virtual ~Component() = default;

public:
	GameObject* gameObject = nullptr;

public:
	// 生成
	virtual void Initialize(GameContext& context) {}
	// 更新
	virtual void Update(GameContext& context) {}
	// 描画
	virtual void Render(GameContext& context) {}
	// 破棄
	virtual void Finalize(GameContext& context) {}
};
