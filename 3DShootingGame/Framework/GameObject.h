#pragma once
#include "Transform.h"
#include "Component.h"
#include "Object.h"
#include "ObjectField.h"
#include <Utilities/TypeId.h>

class GameContext;

#ifdef DONT_USE_ENTT
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
	std::list<ObjectHolder<Component>> m_components;
	std::list<ObjectHolder<Component>> m_addingComponents;
	std::unordered_map<type_id_t, ObjectField<Component>> m_componentMap;
	std::wstring m_name;

public:
	ObjectField<Transform> transform;

private:
	struct Impl
	{
		template<typename U>
		static auto AddComponentOne(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> void
		{
			components[type_id<U>()] = component;
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> std::enable_if_t<hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
			AddComponents<U::Base>(components, component);
		}

		template<typename U>
		static auto AddComponents(std::unordered_map<type_id_t, ObjectField<Component>>& components, const ObjectField<U>& component)
			-> std::enable_if_t<!hasParent<U>::value>
		{
			AddComponentOne<U>(components, component);
		}
	};

public:
	template<typename T, typename... Args>
	ObjectField<T> AddComponent(Args&& ... args)
	{
		auto component = ObjectHolder<T>::Create(std::forward<Args>(args)...);
		auto field = ObjectField<T>(component);
		component->gameObject = this;
		m_addingComponents.push_back(component);
		Impl::AddComponents<T>(m_componentMap, component);
		return field;
	}

	template<typename T>
	ObjectField<T> GetComponent()
	{
		auto key = type_id<T>();
		if (m_componentMap.count(key) > 0)
		{
			auto& component = m_componentMap.at(key);
			if (component)
				if (!component->IsDestroyed())
					return ObjectField<T>(component);
			m_componentMap.erase(key);
		}
		return nullptr;
	}

private:
	GameObject(const std::wstring & name)
		: m_name(name)
	{
		transform = AddComponent<Transform>();
	}

public:
	~GameObject() = default;

	static ObjectHolder<GameObject> Create(const std::wstring & name = L"GameObject")
	{
		return ObjectHolder<GameObject>::CreateFromUniqueSharedPtr(std::shared_ptr<GameObject>(new GameObject(name)));
	}

	std::wstring GetName() const override { return m_name; }
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

		for (auto itr = m_components.begin(); itr != m_components.end();)
		{
			auto& component = *itr;
			if (component->IsDestroyed())
			{
				component->Finalize(context);
				itr = m_components.erase(itr);
			}
			else
			{
				++itr;
			}
		}
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
#else

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

public:
	// 生成
	void Initialize(GameContext& context)
	{
		auto& reg = m_actor.registry;
		for (auto& component : m_actor.)
			component->Initialize(context);
	}

	// 更新
	void Update(GameContext& context)
	{
		for (auto& component : m_addingComponents)
			component->Initialize(context);
		m_components.splice(m_components.end(), std::move(m_addingComponents));

		for (auto& component : m_components)
			component->Update(context);

		for (auto itr = m_components.begin(); itr != m_components.end();)
		{
			auto& component = *itr;
			if (component->IsDestroyed())
			{
				component->Finalize(context);
				itr = m_components.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}

	// 描画
	void Render(GameContext& context)
	{
		for (auto& component : m_components)
			component->Render(context);
	}

	// 破棄
	void Finalize(GameContext& context)
	{
		for (auto& component : m_components)
			component->Finalize(context);
	}
};

#endif
