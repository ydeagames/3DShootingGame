#pragma once
#include <Framework/Component.h>
#include <Framework/ObjectField.h>
#include <Framework/GameContext.h>
#include <Utilities/TypeId.h>

class GameObject;
class PhysXScene;
class GameContext;

// �Q�[���I�u�W�F�N�g�R���e�i
class Scene : public Component, public GameContext
{
private:
	// �q�Q�[���I�u�W�F�N�g
	std::list<ObjectHolder<GameObject>> m_gameObjects;
	// �q�Q�[���I�u�W�F�N�g
	std::list<ObjectHolder<GameObject>> m_addingObjects;
	// �����p
	std::unordered_multimap<std::wstring, ObjectField<GameObject>> m_objectMap;

public:
	// ���O
	std::wstring name = L"Scene";
	// �����X�V���邩
	bool updateBehind = false;
	// ����`�悷�邩
	bool renderBehind = false;
	// �}�E�X���[�h
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
	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);

private:
	GameContext* m_parent;

public:
	virtual GameContext* GetParent()
	{
		return m_parent;
	}

	// �V�[�����擾
	virtual Scene& GetScene()
	{
		return *this;
	};
};