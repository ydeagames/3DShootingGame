#pragma once
#include <Framework/ECS/Component.h>
#include <Framework/PhysX/PhysXCommons.h>

class Collider : public Component
{
public:
	PhysicsMaterial material;

public:
	virtual ~Collider() = default;

public:
	virtual void AddCollider(physx::PxRigidActor* rigid) const = 0;
};

class SphereCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid) const override;
};

class BoxCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid) const override;
};

class CapsuleCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor* rigid) const override;
};

