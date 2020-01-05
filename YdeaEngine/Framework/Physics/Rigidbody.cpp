#include "pch.h"
#include "Rigidbody.h"
#include "Collidable.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/GameObject.h>
#include <Framework/ECS/Scene.h>
#include <Framework/Context/SceneManager.h>
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/PhysX/PhysXScene.h>
#include <Framework/Tags/Tags.h>

void Rigidbody::Start()
{
	gameObject.FindGameObjectWithTag<Tag::PhysXSceneTag>().ifPresent([&](GameObject& obj)
		{
			if (obj.HasComponent<PhysXScene>())
			{
				auto& manager = GameContext::Get<PhysXManager>();
				auto trans = physx::PxTransform(physx::toPhysX(gameObject.GetComponent<Transform>().position), physx::toPhysX(gameObject.GetComponent<Transform>().rotation));
				if (gameObject.GetComponent<Transform>().isStatic)
					rigid = manager.GetPhysics()->createRigidStatic(trans);
				else
				{
					auto dynamic = manager.GetPhysics()->createRigidDynamic(trans);
					dynamic->setRigidBodyFlags(lockFlags);
					rigid = dynamic;
				}

				auto& reg = *gameObject.registry;
				auto& e = gameObject.entity;
				std::vector<entt::entity> src;
				auto rec0 = [&](auto& e, auto& rec) mutable -> void {
					src.push_back(e);
					reg.view<Transform>().each([&](auto entity, Transform& component) {
						if (component.parent == e)
							rec(entity, rec);
						});
				};
				rec0(e, rec0);

				Collidable::AddCollider(reg, src, std::forward<physx::PxRigidActor>(*rigid));

				obj.GetComponent<PhysXScene>().CreateObject(*rigid);

				if (rigid && rigid->is<physx::PxRigidBody>())
				{
					auto dynamic = rigid->is<physx::PxRigidBody>();
					dynamic->setLinearVelocity(preVelocity);
					dynamic->addForce(preForce);
				}
			}
		});
}

void Rigidbody::Update()
{
	if (rigid)
	{
		auto trans = rigid->getGlobalPose();
		gameObject.GetComponent<Transform>().position = physx::fromPhysX(trans.p);
		gameObject.GetComponent<Transform>().rotation = physx::fromPhysX(trans.q);
	}
}

void Rigidbody::OnDestroy()
{
	if (rigid)
	{
		auto scene = rigid->getScene();
		if (scene)
			scene->removeActor(*rigid);
		px_release(rigid);
	}
}

void Rigidbody::AddForce(DirectX::SimpleMath::Vector3 force)
{
	preForce = physx::toPhysX(force);
	if (rigid && rigid->is<physx::PxRigidBody>())
		rigid->is<physx::PxRigidBody>()->addForce(physx::toPhysX(force));
}

void Rigidbody::SetVelocity(DirectX::SimpleMath::Vector3 velocity)
{
	preVelocity = physx::toPhysX(velocity);
	if (rigid && rigid->is<physx::PxRigidBody>())
		rigid->is<physx::PxRigidBody>()->setLinearVelocity(physx::toPhysX(velocity));
}

Transform& Rigidbody::Fetch()
{
	auto& t = gameObject.GetComponent<Transform>();
	if (rigid)
	{
		auto trans = rigid->getGlobalPose();
		t.position = physx::fromPhysX(trans.p);
		t.rotation = physx::fromPhysX(trans.q);
	}
	return t;
}

void Rigidbody::Apply()
{
	if (rigid)
	{
		auto& t = gameObject.GetComponent<Transform>();
		physx::PxTransform trans;
		trans.p = physx::toPhysX(t.position);
		trans.q = physx::toPhysX(t.rotation);
		rigid->setGlobalPose(trans);
	}
}

void Rigidbody::EditorGui()
{
	{
		uint32_t flags = lockFlags;
		ImGui::CheckboxFlags("Kinematic", &flags, physx::PxRigidBodyFlag::eKINEMATIC);
		ImGui::CheckboxFlags("Use Kinematic Target for Scene Queries", &flags, physx::PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES);
		ImGui::CheckboxFlags("Enable CCD", &flags, physx::PxRigidBodyFlag::eENABLE_CCD);
		ImGui::CheckboxFlags("Enable CCD Friction", &flags, physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION);
		ImGui::CheckboxFlags("Enable Pose Integration Preview", &flags, physx::PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW);
		ImGui::CheckboxFlags("Enable Speculative CCD", &flags, physx::PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD);
		ImGui::CheckboxFlags("Enable CCD Max Contact Impulse", &flags, physx::PxRigidBodyFlag::eENABLE_CCD_MAX_CONTACT_IMPULSE);
		ImGui::CheckboxFlags("Return Accelerations", &flags, physx::PxRigidBodyFlag::eRETAIN_ACCELERATIONS);
		lockFlags = physx::PxRigidBodyFlags(physx::PxU8(flags));
	}
}
