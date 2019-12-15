#pragma once
#include <Framework/ECS/EventBus.h>

class Collidable
{
private:
	template<typename T> static void Register0(...) {}
	template<typename T, typename = decltype(&T::AddCollider)>
	static void Register0()
	{
		ECS::EventBus<Collidable, 0, physx::PxRigidActor>::Register<T>(&T::AddCollider);
	}

public:
	template<typename T>
	static void Register()
	{
		Register0<T>();
	}

	template<typename It>
	static void AddCollider(entt::registry& registry, It first, It last, physx::PxRigidActor&& rigid)
	{
		ECS::EventBusRanged<It, Collidable, 0, physx::PxRigidActor>::Post(registry, first, last, std::forward<physx::PxRigidActor>(rigid));
	}
};
