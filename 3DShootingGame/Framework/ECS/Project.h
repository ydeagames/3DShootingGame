// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 06:40:16 +0900
// Modified: 2019-12-15 06:40:16 +0900

#pragma once
#include <Framework/StandardBuildSettings.h>
#include <Game/BuildSettings.h>

namespace ECS
{
	template<typename ... T_args>
	struct connect;

	template<typename ... T1, typename ... T2>
	struct connect<std::tuple<T1...>, std::tuple<T2...>>
	{
		typedef std::tuple<T1..., T2...> type;
	};

	using AllComponents = ECS::ComponentManager<
		ECS::connect<
			StandardBuildSettings::Components,
			BuildSettings::Components
		>::type,
		ECS::connect<
			StandardBuildSettings::Tags,
			BuildSettings::Tags
		>::type,
		ECS::connect<
			StandardBuildSettings::Events,
			BuildSettings::Events
		>::type
	>;
}


