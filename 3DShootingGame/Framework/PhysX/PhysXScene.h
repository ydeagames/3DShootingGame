#pragma once
#include "PhysXCommons.h"
#include <Framework/ECS/Component.h>

class PhysXManager;

class PhysXScene : public Component
{
private:
	PhysXManager* m_manager;
	physx::PxScene* m_scene;

public:
	PhysXScene(PhysXManager& manager);
	~PhysXScene();

	// シーン取得
	physx::PxScene& GetScene();
	// シーンPvd有効化
	void ActivatePvd();
	// オブジェクト追加
	void CreateObject(physx::PxActor& obj);

	// 生成
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Render();
	// 破棄
	void Finalize();
};

