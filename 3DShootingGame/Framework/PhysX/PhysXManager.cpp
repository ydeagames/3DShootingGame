#include "pch.h"
#include "PhysXManager.h"
#include "PhysXScene.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Common/StepTimer.h>

using namespace physx;

PhysXManager::PhysXManager()
{
	m_foundation = px_unique_ptr<PxFoundation>(PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback));

	m_pvd = px_unique_ptr<PxPvd>(PxCreatePvd(*m_foundation));
	m_pvdTransport = px_unique_ptr<PxPvdTransport>(PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10));
	m_pvd->connect(*m_pvdTransport, PxPvdInstrumentationFlag::eALL);

	m_physics = px_unique_ptr<PxPhysics>(PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd.get()));
	PxInitExtensions(*m_physics, m_pvd.get());

	PxTolerancesScale toleranceScale;
	toleranceScale.length = 1;
	toleranceScale.speed = 9.8f * 2;
	assert(toleranceScale.isValid()); // make sure this value is always true
	m_cooking = px_unique_ptr<PxCooking>(PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(toleranceScale)));

	m_dispatcher = px_unique_ptr<PxDefaultCpuDispatcher>(PxDefaultCpuDispatcherCreate(2));
}

PhysXManager::~PhysXManager()
{
	m_dispatcher.reset();
	m_physics.reset();

	m_pvd.reset();
	m_pvdTransport.reset();

	m_foundation.reset();
}
