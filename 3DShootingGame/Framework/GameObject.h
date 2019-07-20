#pragma once
#include "Transform.h"
#include "Component.h"
#include "Object.h"
#include <Utilities/TypeId.h>

class GameContext;

template <typename T>
struct hasParent {
private:
	template <typename U>
	static typename U::Bases test(int);
	template <typename>
	static void test(...);
public:
	enum { value = !std::is_void<decltype(test<T>(0))>::value };
};

// ゲームオブジェクト
class GameObject final : public Object, public std::enable_shared_from_this<GameObject>
{
private:
	std::list<std::shared_ptr<Component>> m_components;
	std::list<std::shared_ptr<Component>> m_addingComponents;
	std::unordered_map<type_id_t, std::weak_ptr<Component>> m_componentMap;

public:
	std::wstring name;
	std::shared_ptr<Transform> transform;

private:
	struct Impl
	{
		template<typename U>
		static auto AddComponentOne(std::unordered_map<type_id_t, std::weak_ptr<Component>>& components, const std::weak_ptr<U>& component)
			-> void
		{
			components[type_id<U>()] = component;
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, std::weak_ptr<Component>>& components, const std::weak_ptr<U>& component)
			-> std::enable_if_t<hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
			AddComponents<U::Base>(components, component);
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, std::weak_ptr<Component>>& components, const std::weak_ptr<U>& component)
			-> std::enable_if_t<!hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
		}
	};

public:
	template<typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&& ... args)
	{
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		component->gameObject = this;
		m_addingComponents.push_back(component);
		Impl::AddComponents<T>(m_componentMap, component);
		return component;
	}

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		auto key = type_id<T>();
		auto& component = m_componentMap[key];
		if (auto lck = component.lock())
			if (!lck->destroyed)
				return std::dynamic_pointer_cast<T>(lck);
		m_componentMap.erase(key);
		return nullptr;
	}

private:
	GameObject(const std::wstring & name)
		: name(name)
	{
		transform = AddComponent<Transform>();
	}

public:
	~GameObject() = default;

	static std::shared_ptr<GameObject> Create(const std::wstring & name = L"GameObject")
	{
		return std::shared_ptr<GameObject>(new GameObject(name));
	}

	std::wstring GetName() const override { return name; }
	std::wstring GetType() const override { return L"GameObject"; }

public:
	// 生成
	void Initialize(GameContext & context)
	{
		for (auto& component : m_components)
			component->Initialize(context);
	}

	// 更新
	void Update(GameContext & context)
	{
		for (auto& component : m_addingComponents)
			component->Initialize(context);
		m_components.splice(m_components.end(), std::move(m_addingComponents));

		for (auto& component : m_components)
			component->Update(context);

		m_components.remove_if(std::mem_fn(&Object::destroyed));
	}

	// 描画
	void Render(GameContext & context)
	{
		for (auto& component : m_components)
			component->Render(context);
	}

	// 破棄
	void Finalize(GameContext & context)
	{
		for (auto& component : m_components)
			component->Finalize(context);
	}
};

