#pragma once
#include <Framework/ECS/EventBus.h>

class Updatable
{
private:
	template<typename T> static void RegisterFirst(...) {}
	template<typename T, typename = decltype(&T::Start)>
	static void RegisterFirst()
	{
		ECS::EventBus<Updatable, 0>::RegisterFirst<T>(&T::Start);
	}

	template<typename T> static void RegisterTick(...) {}
	template<typename T, typename = decltype(&T::Update)>
	static void RegisterTick()
	{
		ECS::EventBus<Updatable, 1>::Register<T>(&T::Update);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterFirst<T>();
		RegisterTick<T>();
	}

	static void Start(entt::registry& registry)
	{
		ECS::EventBus<Updatable, 0>::Post(registry);
	}

	static void Update(entt::registry& registry)
	{
		ECS::EventBus<Updatable, 0>::Post(registry);
		ECS::EventBus<Updatable, 1>::Post(registry);
	}
};
