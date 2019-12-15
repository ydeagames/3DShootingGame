#pragma once
#include <Framework/Components/StandardSerialize.h>
#include <Framework/Events/Updatable.h>
#include <Framework/Events/Renderable.h>
#include <Framework/Components/Transform.h>
#include <Framework/PhysX/PhysXScene.h>
#include <Framework/Tags/Tags.h>
#include <Framework/Physics/Rigidbody.h>
#include <Framework/Physics/Collider.h>
#include <Framework/Physics/Collidable.h>

namespace StandardBuildSettings
{
	using Components = std::tuple<
		Transform,
		PhysXScene,
		Rigidbody,
		Collider
	>;

	using Tags = std::tuple<
		Tag::PhysXScene
	>;

	using Events = std::tuple<
		Updatable,
		Renderable,
		Collidable
	>;
}