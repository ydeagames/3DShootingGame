#include "pch.h"
#include "PhysXManager.h"
#include "PhysXScene.h"
#include "PhysXRenderer.h"
#include <Framework/GameContext.h>
#include <Framework/GameCamera.h>
#include <Common/StepTimer.h>

using namespace physx;

PhysXManager::PhysXManager()
{
}

PhysXManager::~PhysXManager()
{
}

void PhysXManager::Initialize(GameContext& context)
{
	m_foundation = px_unique_ptr<PxFoundation>(PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback));

	m_pvd = px_unique_ptr<PxPvd>(PxCreatePvd(*m_foundation));
	m_pvdTransport = px_unique_ptr<PxPvdTransport>(PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10));
	m_pvd->connect(*m_pvdTransport, PxPvdInstrumentationFlag::eALL);

	m_physics = px_unique_ptr<PxPhysics>(PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd.get()));

	m_dispatcher = px_unique_ptr<PxDefaultCpuDispatcher>(PxDefaultCpuDispatcherCreate(2));
}

void PhysXManager::Update(GameContext& context)
{
	auto nbScenes = m_physics->getNbScenes();
	if (nbScenes)
	{
		std::vector<physx::PxScene*> scenes(nbScenes);
		m_physics->getScenes(&scenes[0], nbScenes);

		for (auto& scene : scenes)
		{
			//scene->simulate(1.0f / 60.0f);
			scene->simulate(float(context.GetTimer().GetElapsedSeconds()));
			scene->fetchResults(true);
		}
	}
}

void PhysXManager::Render(GameContext& context)
{
	auto nbScenes = m_physics->getNbScenes();
	if (nbScenes)
	{
		std::vector<physx::PxScene*> scenes(nbScenes);
		m_physics->getScenes(&scenes[0], nbScenes);

		for (auto& scene : scenes)
		{
			PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
			if (nbActors)
			{
				std::vector<PxRigidActor*> actors(nbActors);
				scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor * *>(&actors[0]), nbActors);
				PhysXRenderer::RenderActors(context, actors, true, physx::PxVec3(0, 0, 1));
			}
		}
	}
}

void PhysXManager::Finalize(GameContext& context)
{
	m_dispatcher.reset();
	m_physics.reset();

	m_pvd.reset();
	m_pvdTransport.reset();

	m_foundation.reset();
}

std::unique_ptr<PhysXScene> PhysXManager::CreateScene()
{
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_dispatcher.get();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	return std::make_unique<PhysXScene>(m_physics->createScene(sceneDesc));
}
