#include "pch.h"
#include "Rigidbody.h"
#include "GameContext.h"
#include "Scene.h"
#include "GameObject.h"
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/PhysX/PhysXScene.h>

void Rigidbody::Initialize(GameContext& context)
{
	auto& manager = context.GetPhysics();
	auto& scene = context.GetPhysicsScene();
	auto trans = physx::PxTransform(physx::toPhysX(gameObject->transform->position), physx::toPhysX(gameObject->transform->rotation));
	if (isStatic)
		rigid = manager.GetPhysics()->createRigidStatic(trans);
	else
	{
		auto dynamic = manager.GetPhysics()->createRigidDynamic(trans);
		for (auto& flagSet : lockFlags)
			dynamic->setRigidDynamicLockFlag(flagSet.first, flagSet.second);
		rigid = dynamic;
	}

	for (auto& collider : colliders)
		collider->AddCollider(context, rigid, gameObject->transform);

	scene.CreateObject(*rigid);

	if (rigid && rigid->is<physx::PxRigidBody>())
	{
		auto dynamic = rigid->is<physx::PxRigidBody>();
		dynamic->setLinearVelocity(preVelocity);
		dynamic->addForce(preForce);
	}
}

void Rigidbody::Update(GameContext& context)
{
	auto trans = rigid->getGlobalPose();
	gameObject->transform->position = physx::fromPhysX(trans.p);
	gameObject->transform->rotation = physx::fromPhysX(trans.q);
}

void Rigidbody::Finalize(GameContext& context)
{
	px_release(rigid);
}
