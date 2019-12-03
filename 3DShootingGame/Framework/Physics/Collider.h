#pragma once
#include <Framework/Components/Components.h>
#include <Framework/PhysX/PhysXCommons.h>

class Collider : public Component
{
public:
	Transform localTransform;
	PhysicsMaterial material;

public:
	virtual ~Collider() = default;

public:
	virtual void AddCollider(physx::PxRigidActor* rigid, const Transform& transform) const = 0;
};

class SphereCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid, const Transform& transform) const override;
};

class BoxCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid, const Transform& transform) const override;
};

class CapsuleCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid, const Transform& transform) const override;
};

