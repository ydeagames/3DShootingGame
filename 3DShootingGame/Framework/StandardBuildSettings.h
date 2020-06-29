// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 06:40:16 +0900
// Modified: 2020-01-09 18:30:53 +0900

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
#include <Framework/SceneManagement/SceneTransitions.h>

namespace StandardBuildSettings
{
	using Components = std::tuple<
		Transform,
		PhysXScene,
		Rigidbody,
		SphereCollider,
		BoxCollider,
		CapsuleCollider,
		PlaneCollider,
		SceneTransitions::FadeTransition,
		SceneTransitions::RuleTransition
	>;

	using Tags = std::tuple<
		Tag::PhysXSceneTag
	>;

	using Events = std::tuple<
		Updatable,
		Renderable,
		Collidable
	>;
}

