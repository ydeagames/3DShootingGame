#include "pch.h"
#include "PhysXScene.h"
#include "PhysXManager.h"

using namespace physx;

PhysXScene::PhysXScene(physx::PxScene* scene)
	: m_scene(scene)
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
