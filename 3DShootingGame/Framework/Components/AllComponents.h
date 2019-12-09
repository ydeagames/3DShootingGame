#pragma once
#include <Framework/ECS/Component.h>
#include "Transform.h"
#include <Game/Components/AllComponents.h>

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
			std::tuple<
				Transform
			>,
			Components
		>::type,
		std::tuple<
		>,
		std::tuple<
			Updatable,
			Renderable
		>
	>;
}
