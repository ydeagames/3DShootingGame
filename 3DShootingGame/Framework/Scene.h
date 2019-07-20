#pragma once
#include "Component.h"

class GameObject;
class PhysXScene;
class GameContext;

// ゲームオブジェクトコンテナ
class Scene : public Component
{
public:
	// 名前
	std::wstring name = L"Scene";
	// 子ゲームオブジェクト
	std::list<std::shared_ptr<GameObject>> gameObjects;
	// 子ゲームオブジェクト
	std::list<std::shared_ptr<GameObject>> addingObjects;
	// 後ろを更新するか
	bool updateBehind = false;
	// 後ろを描画するか
	bool renderBehind = false;
	// マウスモード
	DirectX::Mouse::Mode mouseMode = DirectX::Mouse::Mode::MODE_ABSOLUTE;

public:
	std::wstring GetName() const override { return name; }

private:
	// PhysX
	std::unique_ptr<PhysXScene> physics;

public:
	Scene();
	~Scene();

public:
	void Add(const std::shared_ptr<GameObject>& obj);
	PhysXScene& GetPhysics() const { return *physics; }

public:
	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
	// 描画
	void Render(GameContext& context);
	// 破棄
	void Finalize(GameContext& context);
};