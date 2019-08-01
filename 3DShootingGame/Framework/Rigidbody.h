#pragma once
#include "Component.h"
#include <Utilities/Property.h>
#include <Framework/PhysX/PhysXCommons.h>
#include "Transform.h"
#include "Collider.h"

class GameContext;

class Rigidbody : public Component
{
private:
	physx::PxRigidActor* rigid;
	bool isStatic = false;
	std::unordered_map<physx::PxRigidDynamicLockFlag::Enum, bool> lockFlags;
	std::vector<std::shared_ptr<Collider>> colliders;

private:
	physx::PxVec3 preForce;
	physx::PxVec3 preVelocity;

public:
	void Initialize(GameContext& context);
	void Update(GameContext& context);
	void Finalize(GameContext& context);

	void AddForce(DirectX::SimpleMath::Vector3 force)
	{
		preForce = physx::toPhysX(force);
		if (rigid && rigid->is<physx::PxRigidBody>())
			rigid->is<physx::PxRigidBody>()->addForce(physx::toPhysX(force));
	}

	void SetVelocity(DirectX::SimpleMath::Vector3 velocity)
	{
		preVelocity = physx::toPhysX(velocity);
		if (rigid && rigid->is<physx::PxRigidBody>())
			rigid->is<physx::PxRigidBody>()->setLinearVelocity(physx::toPhysX(velocity));
	}

	void SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
	{
		lockFlags[flag] = value;
	}

	std::shared_ptr<Collider> Add(const std::shared_ptr<Collider>& collider)
	{
		colliders.push_back(collider);
		return collider;
	}

	void SetStatic(bool staticFlag)
	{
		isStatic = staticFlag;
	}

	Property<std::shared_ptr<Transform>> transform = {
		[&]()
		{
			auto t = std::make_shared<Transform>();
			auto trans = rigid->getGlobalPose();
			t->position = physx::fromPhysX(trans.p);
			t->rotation = physx::fromPhysX(trans.q);
			return t;
		},
		[&](const std::shared_ptr<Transform> & value)
		{
			physx::PxTransform trans;
			trans.p = physx::toPhysX(value->position);
			trans.q = physx::toPhysX(value->rotation);
			rigid->setGlobalPose(trans);
		}
	};
};
