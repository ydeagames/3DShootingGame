#pragma once
#include "EventBus.h"

class Camera;

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

	static void Update(Scene& registry)
	{
		ECS::EventBus<Updatable, 0>::Post(registry);
		ECS::EventBus<Updatable, 1>::Post(registry);
	}
};

class Renderable
{
private:
	template<typename T> static void RegisterFirst0(...) {}
	template<typename T, typename = decltype(&T::RenderStart)>
	static void RegisterFirst0()
	{
		ECS::EventBus<Renderable, 0>::RegisterFirst<T>(&T::RenderStart);
	}

	template<typename T> static void Register0(...) {}
	template<typename T, typename = decltype(&T::Render)>
	static void Register0()
	{
		ECS::EventBus<Renderable, 1, Camera>::Register<T>(&T::Render);
	}

	template<typename T> static void RegisterGui0(...) {}
	template<typename T, typename = decltype(&T::RenderGui)>
	static void RegisterGui0()
	{
		ECS::EventBus<Renderable, 2, Camera>::Register<T>(&T::RenderGui);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterFirst0<T>();
		Register0<T>();
		RegisterGui0<T>();
	}

	static void RenderInitialize(Scene& registry)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
	}

	static void Render(Scene& registry, Camera&& camera)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
		ECS::EventBus<Renderable, 1, Camera>::Post(registry, std::forward<Camera>(camera));
	}

	static void RenderGui(Scene& registry, Camera&& camera)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
		ECS::EventBus<Renderable, 2, Camera>::Post(registry, std::forward<Camera>(camera));
	}
};
