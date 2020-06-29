// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-22 06:12:28 +0900
// Modified: 2020-01-17 11:44:41 +0900

#pragma once
#include "Collider.h"
#include <Framework/ECS/Component.h>
#include <Framework/PhysX/PhysXCommons.h>
#include <Framework/Components/Transform.h>

class GameContext;

class Rigidbody : public Component
{
public:
	static constexpr const char* Identifier = "Rigidbody";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	physx::PxRigidActor* rigid = nullptr;

public:
	physx::PxRigidBodyFlags lockFlags;

private:
	physx::PxVec3 preForce;
	physx::PxVec3 preVelocity;

public:
	void Start();
	void Update();
	void OnDestroy();

	void AddForce(const DirectX::SimpleMath::Vector3& force);
	void SetVelocity(const DirectX::SimpleMath::Vector3& velocity);
	DirectX::SimpleMath::Vector3 GetVelocity() const;

	Transform& Fetch();
	void Apply();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(lockFlags));
	}

	void EditorGui();
};


