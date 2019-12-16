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

private:
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

	void AddForce(DirectX::SimpleMath::Vector3 force);
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity);

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
