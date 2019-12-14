#include "pch.h"
#include "GameObject.h"
#include "Component.h"
#include <Framework/Components/Transform.h>

GameObject GameObject::Wrap(entt::entity e) const
{
	return GameObject(registry, e);
}

GameObject GameObject::Create(entt::entity parent)
{
	auto obj = Wrap(registry->create());
	obj.AddComponent<Transform>().parent = parent;
	return obj;
}

Optional<GameObject> GameObject::Find(const std::string& name) const
{
	for (auto& transform : registry->view<Transform>(entt::raw_t{}))
		if (transform.name == name)
			return transform.gameObject;
	return nullptr;
}

std::vector<GameObject> GameObject::FindAll(const std::string& name) const
{
	std::vector<GameObject> objects;
	for (auto& transform : registry->view<Transform>(entt::raw_t{}))
		if (transform.name == name)
			objects.push_back(transform.gameObject);
	return objects;
}

void GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject)
		Destroy(*gameObject);
}

void GameObject::Destroy(GameObject& gameObject)
{
	gameObject.registry->destroy(gameObject.entity);
}

entt::entity GameObject::GetParent() const
{
	if (registry->has<Transform>())
		return registry->get<Transform>().parent;
	return entt::null;
}

std::vector<entt::entity> GameObject::GetChildren() const
{
	std::vector<entt::entity> children;
	registry->view<Transform>().each([&](auto e, Transform& transform) {
		if (transform.parent == entity)
			children.push_back(e);
		});
	return children;
}