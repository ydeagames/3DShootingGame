// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 06:40:16 +0900
// Modified: 2019-12-16 06:22:52 +0900

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


