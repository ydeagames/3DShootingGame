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
	std::vector<GameObject> FindAll(const std::string& name) const;

	template<typename Tag>
	bool HasTag() const
	{
		return registry->has<Tag>(entt::tag_t{}, entity);
	}

	template<typename Tag>
	bool IsTagExists() const
	{
		return registry->has<Tag>();
	}

	template<typename Tag, typename... Args>
	Tag& AddTag(Args&&... args)
	{
		if (registry->has<Tag>())
			registry->remove<Tag>();
		return registry->assign<Tag>(entt::tag_t{}, entity, std::forward<Component>(args)...);
	}

	template<typename Tag>
	Optional<GameObject> FindGameObjectWithTag() const
	{
		if (registry->has<Tag>())
			return Wrap(registry->attachee<Tag>());
		return null;
	}

	static void Destroy(GameObject* gameObject);
	static void Destroy(GameObject& gameObject);

	entt::entity GetParentEntity() const;
	std::vector<entt::entity> GetChildrenEntity() const;
	Optional<GameObject> GetParent() const;
	std::vector<GameObject> GetChildren() const;
};
