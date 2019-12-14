#include "pch.h"
#include "PhysXScene.h"
#include "PhysXManager.h"
#include "PhysXRenderer.h"
#include <Framework/ECS/GameContext.h>

using namespace physx;

PhysXScene::PhysXScene()
{
}

PhysXScene::~PhysXScene()
{
}

physx::PxScene& PhysXScene::GetScene()
{
	return *m_scene;
}

void PhysXScene::ActivatePvd()
{
	auto pvdClient = m_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

void PhysXScene::CreateObject(physx::PxActor& obj)
{
	m_scene->addActor(obj);
}

void PhysXScene::Awake()
{
	auto physics = GameContext::Get<PhysXManager>().GetPhysics();
	auto sceneDesc = PxSceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f * 2, 0.0f);
	sceneDesc.cpuDispatcher = GameContext::Get<PhysXManager>().GetDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_scene = physics->createScene(sceneDesc);
}

void PhysXScene::Update()
{
	m_scene->simulate(1.0f / 60.0f);
	//std::cout << float(context.GetTimer().GetElapsedSeconds()) << std::endl;
	//scene->simulate(float(context.GetTimer().GetElapsedSeconds()));
	m_scene->fetchResults(true);
}

void PhysXScene::Render(GameCamera& camera)
{
	if (GameContext::Get<PhysXManager>().debugMode & PhysXManager::IngamePvdMode::Collision)
	{
		PxU32 nbActors = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor * *>(&actors[0]), nbActors);
			PhysXRenderer::RenderActors(actors, true, physx::PxVec3(0, 0, 1));
		}
	}
}
