#pragma once
#include <Framework/ECS/EventBus.h>

class GameCamera;

class Renderable
{
private:
	template<typename T> static void RegisterFirst0(...) {}
	template<typename T, typename = decltype(&T::RenderStart)>
	static void RegisterFirst0()
	{
		ECS::EventBus<Renderable, 0>::RegisterFirst<T>(&T::RenderStart);
	}

	template<typename T> static void RegisterPre0(...) {}
	template<typename T, typename = decltype(&T::OnPreRender)>
	static void RegisterPre0()
	{
		ECS::EventBus<Renderable, 3, GameCamera>::Register<T>(&T::OnPreRender);
	}

	template<typename T> static void Register0(...) {}
	template<typename T, typename = decltype(&T::Render)>
	static void Register0()
	{
		ECS::EventBus<Renderable, 1, GameCamera>::Register<T>(&T::Render);
	}

	template<typename T> static void RegisterGui0(...) {}
	template<typename T, typename = decltype(&T::RenderGui)>
	static void RegisterGui0()
	{
		ECS::EventBus<Renderable, 2, GameCamera>::Register<T>(&T::RenderGui);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterFirst0<T>();
		RegisterPre0<T>();
		Register0<T>();
		RegisterGui0<T>();
	}

	static void RenderStart(entt::registry& registry)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
	}

	static void Render(entt::registry& registry, GameCamera&& camera)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
		ECS::EventBus<Renderable, 3, GameCamera>::Post(registry, std::forward<GameCamera>(camera));
		ECS::EventBus<Renderable, 1, GameCamera>::Post(registry, std::forward<GameCamera>(camera));
	}

	static void RenderGui(entt::registry& registry, GameCamera&& camera)
	{
		ECS::EventBus<Renderable, 0>::Post(registry);
		ECS::EventBus<Renderable, 2, GameCamera>::Post(registry, std::forward<GameCamera>(camera));
	}
};
