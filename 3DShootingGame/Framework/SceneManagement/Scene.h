#pragma once
#include <Framework/Component.h>
#include <Framework/ObjectField.h>
#include <Framework/GameContext.h>
#include <Utilities/TypeId.h>

class GameObject;
class PhysXScene;
class GameContext;

// ゲームオブジェクトコンテナ
class Scene : public Component, public GameContext
{
private:
	// 子ゲームオブジェクト
	std::list<ObjectHolder<GameObject>> m_gameObjects;
	// 子ゲームオブジェクト
	std::list<ObjectHolder<GameObject>> m_addingObjects;
	// 検索用
	std::unordered_multimap<std::wstring, ObjectField<GameObject>> m_objectMap;
	// Scene
	entt::DefaultRegistry m_registry;

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

public:
	Scene();
	~Scene();

public:
	ObjectField<GameObject> Add(ObjectHolder<GameObject>&& obj);
	ObjectField<GameObject> Find(const std::wstring& name);
	std::list<ObjectField<GameObject>> FindAll(const std::wstring& name);
	ObjectField<GameObject> AddGameObject(const std::wstring& objname = L"GameObject");
	std::list<ObjectHolder<GameObject>>& GetObjects() { return m_gameObjects; }

public:
	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
	// 描画
	void Render(GameContext& context);
	// 破棄
	void Finalize(GameContext& context);

private:
	GameContext* m_parent;

public:
	virtual GameContext* GetParent()
	{
		return m_parent;
	}

	// シーンを取得
	virtual Scene& GetScene()
	{
		return *this;
	};
};
