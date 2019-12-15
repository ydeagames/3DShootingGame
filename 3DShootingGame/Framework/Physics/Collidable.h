#pragma once
#include <Framework/ECS/EventBus.h>

class Collidable
{
private:
	template<typename T> static void Register0(...) {}
	template<typename T, typename = decltype(&T::AddCollider)>
	static void Register0()
	{
		ECS::EventBus<Collidable, 0, physx::PxRigidActor>::Register<T>(&T::Update);
	}

public:
	template<typename T>
	static void Register()
	{
		Register0<T>();
	}

	static void AddCollider(entt::registry& registry, physx::PxRigidActor&& rigid)
	{
		ECS::EventBus<Collidable, 0, physx::PxRigidActor>::Post(registry, std::forward<physx::PxRigidActor>(rigid));
	}
};
