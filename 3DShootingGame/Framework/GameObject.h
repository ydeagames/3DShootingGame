#pragma once
#include "Transform.h"
#include "Component.h"
#include "Object.h"
#include "ObjectField.h"
#include <Utilities/TypeId.h>

class GameContext;

// ゲームオブジェクト
class GameObject final : public Object, public std::enable_shared_from_this<GameObject>
{
private:
	entt::DefaultActor m_actor;
	std::wstring m_name;

public:
	std::reference_wrapper<Transform> transform;

public:
	template<typename T, typename... Args>
	T& AddComponent(Args&& ... args)
	{
		return m_actor.assign<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		return m_actor.get<T>();
	}

private:
	GameObject(entt::DefaultRegistry& scene, const std::wstring& name)
		: m_name(name)
		, m_actor(entt::DefaultActor(scene))
		, transform(std::ref(AddComponent<Transform>()))
	{
	}

public:
	~GameObject() = default;

	static ObjectHolder<GameObject> Create(entt::DefaultRegistry& scene, const std::wstring& name = L"GameObject")
	{
		return ObjectHolder<GameObject>::CreateFromUniqueSharedPtr(std::shared_ptr<GameObject>(new GameObject(scene, name)));
	}

	std::wstring GetName() const override { return m_name; }
	std::wstring GetType() const override { return L"GameObject"; }
};
