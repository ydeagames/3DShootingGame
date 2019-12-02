#pragma once
#include "Scene.h"
#include "GameObject.h"

class GameContext;

namespace ECS
{
	template<typename Events, uint64_t meta = 0, typename... Args>
	class EventBus
	{
	private:
		template<typename T>
		struct started_t {};

		using Func = void(Scene& registry, Args&& ... args);
		static std::vector<std::function<Func>>& handlers()
		{
			static std::vector<std::function<Func>> value;
			return value;
		}

		template<typename T, typename F>
		static void RegisterCustomOnce(F f)
		{
			handlers().push_back([f](Scene& registry, auto&& ... args) {
				registry.registry.view<T>().each([f, &registry, &args](auto& entity, T& comp) {
					f(registry.registry, entity, comp, std::forward<Args>(args)...);
					});
				});
		}

		template<typename T, typename... F>
		static void RegisterOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& registry, auto& entity, T& comp, auto&& ... args) {
				using accumulator_type = int[];
				accumulator_type accumulator = { 0, ((comp.*f)(args...), 0)... };
				(void)accumulator;
			});
		}

		template<typename T, typename... F>
		static void RegisterFirstOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& registry, auto& entity, T& comp, auto&& ... args) {
				if (!registry.has<started_t<T>>(entity))
				{
					struct Listener
					{
						void on(entt::registry& registry, entt::entity entity)
						{
							registry.reset<started_t<T>>(entity);
						}
					} listener;
					registry.destruction<T>().connect<Listener, & Listener::on>(&listener);
					registry.assign<started_t<T>>(entity);
					{
						using accumulator_type = int[];
						accumulator_type accumulator = { 0, ((comp.*f)(args...), 0)... };
						(void)accumulator;
					}
				}
			});
		}

	public:
		static void Post(Scene& registry, Args&& ... args)
		{
			for (auto& func : handlers())
				func(registry, std::forward<Args>(args)...);
		}

	public:
		template<typename T, typename F>
		static void Register(F f)
		{
			static int once = (RegisterOnce<T>(f), 0);
		}

		template<typename T, typename F>
		static void RegisterCustom(F f)
		{
			static int once = (RegisterCustomOnce<T>(f), 0);
		}

		template<typename T, typename F>
		static void RegisterFirst(F f)
		{
			static int once = (RegisterFirstOnce<T>(f), 0);
		}
	};

	template<typename Registry>
	class LifecycleEvents
	{
	private:
		template<typename Component, typename = decltype(&Component::gameObject)>
		static void Init0(int, Registry & registry)
		{
			class Creation
			{
			public:
				void on(Registry& registry, typename Registry::entity_type entity)
				{
					registry.get<Component>(entity).gameObject = GameObject{ &registry, entity };
				}
			};
			Creation cre;
			registry.construction<Component>().connect<Creation, & Creation::on>(&cre);
		}

		template<typename Component>
		static void Init0(bool, Registry& reg)
		{
		}

		template<typename Component, typename = decltype(&Component::Awake)>
		static void Awake0(int, Registry & registry)
		{
			class Creation
			{
			public:
				void on(Registry& registry, typename Registry::entity_type entity)
				{
					registry.get<Component>(entity).Awake();
				}
			};
			Creation cre;
			registry.construction<Component>().connect<Creation, & Creation::on>(&cre);
		}

		template<typename Component>
		static void Awake0(bool, Registry& reg)
		{
		}

		template<typename Component, typename = decltype(&Component::OnDestroy)>
		static void OnDestroy0(int, Registry & registry)
		{
			class Deletion
			{
			public:
				void on(Registry& registry, typename Registry::entity_type entity)
				{
					registry.get<Component>(entity).OnDestroy();
				}
			};
			Deletion del;
			registry.destruction<Component>().connect<Deletion, & Deletion::on>(&del);
		}

		template<typename Component>
		static void OnDestroy0(bool, Registry& reg)
		{
		}

		template<typename Component>
		static void Lifecycle0(Registry& reg)
		{
			Awake0<Component>(0, reg);
			Init0<Component>(0, reg);
			OnDestroy0<Component>(0, reg);
		}

	public:
		template<typename... Components>
		static void Lifecycle(Registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (Lifecycle0<Components>(reg), 0)... };
			(void)accumulator;
		}
	};
}