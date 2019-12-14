#pragma once
#include <Framework/Events/Updatable.h>
#include <Framework/Events/Renderable.h>
#include <Framework/Components/Transform.h>
#include <Framework/PhysX/PhysXScene.h>

namespace StandardBuildSettings
{
	using Components = std::tuple<
		Transform,
		PhysXScene
	>;

	using Tags = std::tuple<
	>;

	using Events = std::tuple<
		Updatable,
		Renderable
	>;
}