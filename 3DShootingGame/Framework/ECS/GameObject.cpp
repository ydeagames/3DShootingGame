// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 11:08:16 +0900

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

entt::entity GameObject::GetParentEntity() const
{
	if (registry->has<Transform>(entity))
		return registry->get<Transform>(entity).parent;
	return entt::null;
}

std::vector<entt::entity> GameObject::GetChildrenEntity() const
{
	std::vector<entt::entity> children;
	registry->view<Transform>().each([&](auto e, Transform& transform) {
		if (transform.parent == entity)
			children.push_back(e);
		});
	return children;
}

Optional<GameObject> GameObject::GetParent() const
{
	if (registry->has<Transform>())
		return Wrap(registry->get<Transform>().parent);
	return nullptr;
}

std::vector<GameObject> GameObject::GetChildren() const
{
	std::vector<GameObject> children;
	registry->view<Transform>(entt::raw_t{}).each([&](Transform& transform) {
		if (transform.parent == entity)
			children.push_back(transform.gameObject);
		});
	return children;
}


