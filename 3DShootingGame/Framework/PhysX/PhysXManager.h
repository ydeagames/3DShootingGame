// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-08 03:19:28 +0900

#pragma once
#include "PhysXCommons.h"
#include <Framework/ECS/Component.h>

class GameContext;
class PhysXScene;

class PhysXManager
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
};



