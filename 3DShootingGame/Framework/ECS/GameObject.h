#pragma once
#include <Utilities/Optional.h>

class GameObject final
{
public:
	entt::registry* registry;
	entt::entity entity;

public:
	GameObject(entt::registry* registry, entt::entity entity)
		: registry(registry)
		, entity(entity)
	{
	}

	GameObject()
		: registry(nullptr)
		, entity(entt::null)
	{
	}

public:
	template<typename Component>
	bool HasComponent() const
	{
		return registry->has<Component>(entity);
	}

	template<typename Component, typename... Args>
	Component& AddComponent(Args&&... args)
	{
		return registry->assign<Component>(entity, std::forward<Component>(args)...);
	}

	template<typename Component>
	const Component& GetComponent() const
	{
		return registry->get<Component>(entity);
	}

	template<typename Component>
	inline Component& GetComponent()
	{
		return registry->get<Component>(entity);
	}

	GameObject Wrap(entt::entity e) const;
	GameObject Create(entt::entity parent = entt::null);
	Optional<GameObject> Find(const std::string& name) const;

	static void Destroy(GameObject* gameObject);
	static void Destroy(GameObject& gameObject);

	// TODO to GameObject
	entt::entity GetParent() const;
	std::vector<entt::entity> GetChildren() const;
};
