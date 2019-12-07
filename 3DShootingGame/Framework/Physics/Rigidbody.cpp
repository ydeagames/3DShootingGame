#include "pch.h"
#include "Rigidbody.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/GameObject.h>
#include <Framework/ECS/Scene.h>
#include <Framework/Context/SceneManager.h>
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/PhysX/PhysXScene.h>

void Rigidbody::Initialize()
{
	auto& manager = GameContext::Get<PhysXManager>();
	auto& scene = GameContext::Get<PhysXScene>();
	auto trans = physx::PxTransform(physx::toPhysX(gameObject.GetComponent<Transform>().position), physx::toPhysX(gameObject.GetComponent<Transform>().rotation));
	if (isStatic)
		rigid = manager.GetPhysics()->createRigidStatic(trans);
	else
	{
		auto dynamic = manager.GetPhysics()->createRigidDynamic(trans);
		for (auto& flagSet : lockFlags)
			dynamic->setRigidDynamicLockFlag(flagSet.first, flagSet.second);
		rigid = dynamic;
	}

	//for (auto& collider : colliders)
	//	collider->AddCollider(rigid, gameObject.GetComponent<Transform>());

	scene.CreateObject(*rigid);

	if (rigid && rigid->is<physx::PxRigidBody>())
	{
		auto dynamic = rigid->is<physx::PxRigidBody>();
		dynamic->setLinearVelocity(preVelocity);
		dynamic->addForce(preForce);
	}
}

void Rigidbody::Update()
{
	auto trans = rigid->getGlobalPose();
	gameObject.GetComponent<Transform>().position = physx::fromPhysX(trans.p);
	gameObject.GetComponent<Transform>().rotation = physx::fromPhysX(trans.q);
}

void Rigidbody::Finalize()
{
	auto scene = rigid->getScene();
	if (scene)
		scene->removeActor(*rigid);
	px_release(rigid);
}
