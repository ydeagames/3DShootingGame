#include "pch.h"
#include "Collider.h"
#include "GameContext.h"
#include <Framework/PhysX/PhysXManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void SphereCollider::AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const
{
	auto& manager = context.GetPhysics();
	auto mat = manager.CreateMaterial(material);
	auto t = localTransform;
	t.parent = *transform;
	auto size = *t.lossyScale ;

	auto diameter = (size.x + size.y + size.z) / 3;
	auto geo = physx::PxSphereGeometry(diameter / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	shape->setLocalPose(physx::PxTransform(physx::toPhysX(localTransform.position), physx::toPhysX(localTransform.rotation)));
	rigid->attachShape(*shape);
}

void BoxCollider::AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const
{
	auto& manager = context.GetPhysics();
	auto mat = manager.CreateMaterial(material);
	auto t = localTransform;
	t.parent = *transform;
	auto size = *t.lossyScale;

	auto geo = physx::PxBoxGeometry(physx::toPhysX(size) / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	shape->setLocalPose(physx::PxTransform(physx::toPhysX(localTransform.position), physx::toPhysX(localTransform.rotation)));
	rigid->attachShape(*shape);
}

void CapsuleCollider::AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const
{
	auto& manager = context.GetPhysics();
	auto mat = manager.CreateMaterial(material);
	auto t = localTransform;
	t.parent = *transform;
	auto size = *t.lossyScale;

	auto diameter = (size.x + size.z) / 2;
	auto geo = physx::PxCapsuleGeometry(diameter / 2, size.y / 2);
	auto shape = manager.GetPhysics()->createShape(geo, *mat);
	shape->setLocalPose(physx::PxTransform(physx::toPhysX(localTransform.position), physx::toPhysX(localTransform.rotation)));
	rigid->attachShape(*shape);
}
