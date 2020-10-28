// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 02:33:31 +0900

#pragma once
#include "GameObject.h"

class GameContext;

namespace ECS
{
	template<typename Events, uint64_t meta = 0, typename... Args>
	class EventBusImplAll
	{
	public:
		using Func = void(entt::registry& registry, Args&& ... args);
		static std::vector<std::function<Func>>& handlers()
		{
			static std::vector<std::function<Func>> value;
			return value;
		}

		template<typename T, typename F>
		static void RegisterCustomOnce(F f)
		{
			handlers().push_back([f](entt::registry& registry, auto&& ... args) {
				registry.view<T>().each([f, &registry, &args...](auto& entity, T& comp) {
					f(registry, entity, comp, std::forward<Args>(args)...);
					});
				});
		}
	};

	template<typename It, typename Events, uint64_t meta = 0, typename... Args>
	class EventBusImplRanged
	{
	public:
		using Func = void(entt::registry& registry, It first, It last, Args&& ... args);
		static std::vector<std::function<Func>>& handlers()
		{
			static std::vector<std::function<Func>> value;
			return value;
		}

		template<typename T, typename F>
		static void RegisterCustomOnce(F f)
		{
			handlers().push_back([f](entt::registry& registry, It first, It last, auto&& ... args) {
				for (auto itr = first; itr != last; ++itr)
				{
					auto& entity = *itr;
					if (registry.has<T>(entity))
						f(registry, entity, registry.get<T>(entity), std::forward<Args>(args)...);
				}
				});
		}
	};

	template<typename Impl, typename Events, uint64_t meta = 0, typename... Args>
	class EventBusBase
	{
	private:
		template<typename T>
		struct started_t {};

		template<typename T, typename... F>
		static void RegisterOnce(F&& ... f)
		{
			Impl::RegisterCustomOnce<T>([f...](auto& registry, auto& entity, T& comp, auto&& ... args) {
				using accumulator_type = int[];
				accumulator_type accumulator = { 0, ((comp.*f)(args...), 0)... };
				(void)accumulator;
			});
		}

		template<typename T, typename... F>
		static void RegisterFirstOnce(F&& ... f)
		{
			Impl::RegisterCustomOnce<T>([f...](auto& registry, auto& entity, T& comp, auto&& ... args) {
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
		static void Post(entt::registry& registry, Args&& ... args)
		{
			for (auto& func : Impl::handlers())
				func(registry, std::forward<Args>(args)...);
		}

		template<typename It>
		static void Post(entt::registry& registry, It first, It last, Args&& ... args)
		{
			for (auto& func : Impl::handlers())
				func(registry, first, last, std::forward<Args>(args)...);
		}

	public:
		template<typename T, typename F>
		static void Register(F f)
		{
			static int once = (RegisterOnce<T>(f), 0);
		}

		template<typename T, typename F>
		static void RegisterFirst(F f)
		{
			static int once = (RegisterFirstOnce<T>(f), 0);
		}
	};

	template<typename Events, uint64_t meta = 0, typename... Args>
	using EventBus = EventBusBase<EventBusImplAll<Events, meta, Args...>, Events, meta, Args...>;

	template<typename It, typename Events, uint64_t meta = 0, typename... Args>
	using EventBusRanged = EventBusBase<EventBusImplRanged<It, Events, meta, Args...>, Events, meta, Args...>;

	class LifecycleEvents
	{
	private:
		template<typename Component, typename = decltype(&Component::gameObject)>
		static void Init0(int, entt::registry & registry)
		{
			class Creation
			{
			public:
				void on(entt::registry& registry, entt::entity entity)
				{
					registry.get<Component>(entity).gameObject = GameObject{ &registry, entity };
				}
			};
			Creation cre;
			registry.construction<Component>().connect<Creation, & Creation::on>(&cre);
		}

		template<typename Component>
		static void Init0(bool, entt::registry& reg)
		{
		}

		template<typename Component, typename = decltype(&Component::Awake)>
		static void Awake0(int, entt::registry & registry)
		{
			class Creation
			{
			public:
				void on(entt::registry& registry, entt::entity entity)
				{
					registry.get<Component>(entity).Awake();
				}
			};
			Creation cre;
			registry.construction<Component>().connect<Creation, & Creation::on>(&cre);
		}

		template<typename Component>
		static void Awake0(bool, entt::registry& reg)
		{
		}

		template<typename Component, typename = decltype(&Component::OnDestroy)>
		static void OnDestroy0(int, entt::registry & registry)
		{
			class Deletion
			{
			public:
				void on(entt::registry& registry, entt::entity entity)
				{
					registry.get<Component>(entity).OnDestroy();
				}
			};
			Deletion del;
			registry.destruction<Component>().connect<Deletion, & Deletion::on>(&del);
		}

		template<typename Component>
		static void OnDestroy0(bool, entt::registry& reg)
		{
		}

		template<typename Component>
		static void Lifecycle0(entt::registry& reg)
		{
			Awake0<Component>(0, reg);
			Init0<Component>(0, reg);
			OnDestroy0<Component>(0, reg);
		}

	public:
		template<typename... Components>
		static void Lifecycle(entt::registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (Lifecycle0<Components>(reg), 0)... };
			(void)accumulator;
		}
	};
}

