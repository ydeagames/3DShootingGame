// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 17:48:41 +0900
// Modified: 2019-12-16 05:32:44 +0900

#pragma once
#include <Framework/ECS/EventBus.h>

class Collidable
{
public:
	using collection_type = std::vector<entt::entity>;

private:
	template<typename T> static void Register0(...) {}
	template<typename T, typename = decltype(&T::AddCollider)>
	static void Register0()
	{
		ECS::EventBusRanged<collection_type::iterator, Collidable, 0, physx::PxRigidActor>::Register<T>(&T::AddCollider);
	}

public:
	template<typename T>
	static void Register()
	{
		Register0<T>();
	}

	static void AddCollider(entt::registry& registry, collection_type& collection, physx::PxRigidActor&& rigid)
	{
		ECS::EventBusRanged<collection_type::iterator, Collidable, 0, physx::PxRigidActor>::Post(registry, collection.begin(), collection.end(), std::forward<physx::PxRigidActor>(rigid));
	}
};


