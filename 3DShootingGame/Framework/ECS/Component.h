#pragma once
#include "GameObject.h"
#include "ComponentManager.h"

class Component
{
public:
	static constexpr const char* Identifier = "Component";
	GameObject gameObject;

public:
	template<typename Component>
	void Destroy(Component& component)
	{
		gameObject.registry->destroy<Component>(gameObject.entity);
	}
};
