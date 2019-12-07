#pragma once
#include "Object.h"

class GameContext;
class GameObject;

#define COMPONENT(TYPE) \
	public: \
		using Base = TYPE;

// �R���|�[�l���g
class Component : public Object
{
public:
	Component() = default;
	virtual ~Component() = default;

public:
	GameObject* gameObject = nullptr;

	std::wstring GetName() const override { return L"Component"; }
	std::wstring GetType() const override { return L"Component"; }

public:
	// ����
	virtual void Initialize(GameContext& context) {}
	// �X�V
	virtual void Update(GameContext& context) {}
	// �`��
	virtual void Render(GameContext& context) {}
	// �j��
	virtual void Finalize(GameContext& context) {}
};