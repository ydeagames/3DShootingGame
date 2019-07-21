#pragma once
#include "Component.h"
#include "ObjectField.h"
#include <Utilities/TypeId.h>

class GameObject;
class PhysXScene;
class GameContext;

// ゲームオブジェクトコンテナ
class Scene : public Component
{
private:
	// 子ゲームオブジェクト
	std::list<ObjectHolder<GameObject>> gameObjects;
	// 子ゲームオブジェクト
	std::list<ObjectHolder<GameObject>> addingObjects;
	// 検索用
	std::unordered_multimap<type_id_t, std::weak_ptr<GameObject>> m_objectMap;

public:
	// 名前
	std::wstring name = L"Scene";
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
	ObjectField<GameObject> Add(ObjectHolder<GameObject>&& obj);
	template<typename... T> ObjectField<GameObject> AddGameObject(T&&... args) { return Add(GameObject::Create(std::forward(args)...)); }
	PhysXScene& GetPhysics() const { return *physics; }
	std::list<ObjectHolder<GameObject>>& GetObjects() { return gameObjects; }

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
