#pragma once
#include <Framework/ECS/Component.h>
#include <Framework/PhysX/PhysXCommons.h>

class Collider : public Component
{
public:
	PhysicsMaterial material;
};

class SphereCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

class BoxCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

class CapsuleCollider : public Collider
{
public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

