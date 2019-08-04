#pragma once
#include <Framework/Transform.h>
#include <Framework/PhysX/PhysXCommons.h>
#include <Framework/ObjectField.h>

class Collider
{
public:
	Transform localTransform;
	PhysicsMaterial material;

public:
	virtual ~Collider() = default;

public:
	virtual void AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const = 0;
};

class SphereCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const override;
};

class BoxCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const override;
};

class CapsuleCollider : public Collider
{
public:
	void AddCollider(GameContext& context, physx::PxRigidActor* rigid, const ObjectField<Transform>& transform) const override;
};

