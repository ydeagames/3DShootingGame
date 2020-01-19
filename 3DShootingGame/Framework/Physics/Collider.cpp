#include "pch.h"
#include "Collider.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Framework/PhysX/PhysXManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void SphereCollider::AddCollider(physx::PxRigidActor& rigid) const
{
	auto& manager = GameContext::Get<PhysXManager>();
	auto mat = manager.CreateMaterial(material);
	auto& transform = gameObject.GetComponent<Transform>();
	auto size = transform.lossyScale;

	auto diameter = (std::abs(size.x) + std::abs(size.y) + std::abs(size.z)) / 3;
	auto geo = physx::PxSphereGeometry(diameter / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	//shape->setLocalPose(physx::PxTransform(physx::toPhysX(transform.position), physx::toPhysX(transform.rotation)));
	rigid.attachShape(*shape);
}

void BoxCollider::AddCollider(physx::PxRigidActor& rigid) const
{
	auto& manager = GameContext::Get<PhysXManager>();
	auto mat = manager.CreateMaterial(material);
	auto& transform = gameObject.GetComponent<Transform>();
	auto size = transform.lossyScale;
	size = Vector3(std::abs(size.x), std::abs(size.y), std::abs(size.z));

	auto geo = physx::PxBoxGeometry(physx::toPhysX(size) / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	//shape->setLocalPose(physx::PxTransform(physx::toPhysX(transform.position), physx::toPhysX(transform.rotation)));
	rigid.attachShape(*shape);
}

void CapsuleCollider::AddCollider(physx::PxRigidActor& rigid) const
{
	auto& manager = GameContext::Get<PhysXManager>();
	auto mat = manager.CreateMaterial(material);
	auto& transform = gameObject.GetComponent<Transform>();
	auto size = transform.lossyScale;

	auto diameter = (std::abs(size.x) + std::abs(size.z)) / 2;
	auto geo = physx::PxCapsuleGeometry(diameter / 2, std::abs(size.y) / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	//shape->setLocalPose(physx::PxTransform(physx::toPhysX(transform.position), physx::toPhysX(transform.rotation)));
	rigid.attachShape(*shape);
}

void PlaneCollider::AddCollider(physx::PxRigidActor& rigid) const
{
	auto& manager = GameContext::Get<PhysXManager>();
	auto mat = manager.CreateMaterial(material);
	auto& transform = gameObject.GetComponent<Transform>();

	auto geo = physx::PxPlaneGeometry();
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	shape->setLocalPose(physx::PxTransform(physx::toPhysX(Vector3::Zero), physx::toPhysX(Quaternion::CreateFromAxisAngle(Vector3::UnitZ, XMConvertToRadians(90)))));
	rigid.attachShape(*shape);
}

void Collider::EditorGui()
{
}
