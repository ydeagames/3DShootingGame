// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-23 07:07:20 +0900

#pragma once
#include "PhysXCommons.h"
#include <Framework/ECS/Component.h>

class GameCamera;
class PhysXManager;

class PhysXScene : public Component, public physx::PxContactModifyCallback, public physx::PxCCDContactModifyCallback
{
public:
	static constexpr const char* Identifier = "PhysXScene";

private:
	physx::PxScene* m_scene;

public:
	PhysXScene();
	~PhysXScene();

	// シーン取得
	physx::PxScene& GetScene();
	// シーンPvd有効化
	void ActivatePvd();
	// オブジェクト追加
	void CreateObject(physx::PxActor& obj);

	// 生成
	void Awake();
	// 更新
	void Update();
	// 描画
	void Render(GameCamera& camera);

	void onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count) override;
	void onCCDContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count) override;
};



