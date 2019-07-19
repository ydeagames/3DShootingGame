#pragma once
#include "PhysXCommons.h"

class PhysXManager;

class PhysXScene
{
private:
	physx::PxScene* m_scene;

public:
	PhysXScene(physx::PxScene* scene);
	~PhysXScene();

	// シーン取得
	physx::PxScene& GetScene();
	// シーンPvd有効化
	void ActivatePvd();
	// オブジェクト追加
	void CreateObject(physx::PxActor& obj);
};

