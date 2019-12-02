#pragma once
#include "PhysXCommons.h"
#include <Framework/ECS/Component.h>

class GameContext;
class PhysXScene;

class PhysXManager : public Component
{
public:
	enum IngamePvdMode
	{
		Game = 0x1,
		Collision = 0x2,
		GameCollision = 0x3,
	};
	IngamePvdMode debugMode = IngamePvdMode::Game;

private:
	physx::PxDefaultAllocator						m_allocator;
	physx::PxDefaultErrorCallback					m_errorCallback;

	px_unique_ptr<physx::PxPvdTransport>			m_pvdTransport;
	px_unique_ptr<physx::PxPvd>						m_pvd;

	px_unique_ptr<physx::PxDefaultCpuDispatcher>	m_dispatcher;

	px_unique_ptr<physx::PxCooking>					m_cooking;
	px_unique_ptr<physx::PxPhysics>					m_physics;
	px_unique_ptr<physx::PxFoundation>				m_foundation;

public:
	PhysXManager();
	~PhysXManager();

public:
	physx::PxPhysics* GetPhysics()
	{
		return m_physics.get();
	}

	physx::PxCooking* GetCooking()
	{
		return m_cooking.get();
	}

	physx::PxPvd* GetPvd()
	{
		return m_pvd.get();
	}

	physx::PxCpuDispatcher* GetDispatcher()
	{
		return m_dispatcher.get();
	}

	physx::PxMaterial* CreateMaterial(const PhysicsMaterial& material)
	{
		auto mat = m_physics->createMaterial(material.staticFriction, material.dynamicFriction, material.bounciness);
		mat->setFrictionCombineMode(material.frictionCombine);
		mat->setRestitutionCombineMode(material.bounceCombine);
		return mat;
	}

	// ê∂ê¨
	void Initialize(GameContext& context);
	// çXêV
	void Update(GameContext& context);
	// ï`âÊ
	void Render(GameContext& context);
	// îjä¸
	void Finalize(GameContext& context);
};

