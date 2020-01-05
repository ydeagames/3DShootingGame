#pragma once
#include <Framework/ECS/Component.h>
#include <Framework/PhysX/PhysXCommons.h>

class Collider : public Component
{
public:
	PhysicsMaterial material = PhysicsMaterials::Wood;

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

	void EditorGui();
};

class SphereCollider : public Collider
{
public:
	static constexpr const char* Identifier = "SphereCollider";

public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

class BoxCollider : public Collider
{
public:
	static constexpr const char* Identifier = "BoxCollider";

public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

class CapsuleCollider : public Collider
{
public:
	static constexpr const char* Identifier = "CapsuleCollider";

public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

class PlaneCollider : public Collider
{
public:
	static constexpr const char* Identifier = "PlaneCollider";

public:
	void AddCollider(physx::PxRigidActor& rigid) const;
};

