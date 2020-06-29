// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 02:33:31 +0900

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


