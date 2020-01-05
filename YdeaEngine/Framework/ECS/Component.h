#pragma once
#include "GameObject.h"
#include "ComponentManager.h"

class Component
{
public:
	GameObject gameObject;

public:
	template<typename Component>
	void Destroy(Component& component)
	{
		gameObject.registry->destroy<Component>(gameObject.entity);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};
