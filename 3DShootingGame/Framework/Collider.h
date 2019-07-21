#pragma once
#include "Transform.h"
#include <Framework/PhysX/PhysXCommons.h>

class Collider
{
public:
	Transform localTransform;
	PhysicsMaterial material;

public:
	virtual ~Collider() = default;

public:
	virtual void AddCollider(GameContext& context, physx::PxRigidActor* rigid) const = 0;
};

class SphereCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid) const override;
};

class BoxCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid) const override;
};

class CapsuleCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid) const override;
};

