// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-08 01:27:19 +0900

#include <Framework/ECS/Component.h>
#include "DebugCamera.h"

class DebugCamera;
class GameCamera;

class DebugCameraWrapper : public Component
{
private:
	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	// カメラ
	GameCamera* m_camera = nullptr;
public:
	DebugCameraWrapper();
	// Get
	DebugCamera& GetDebugCamera();
	// 生成
	void Initialize();
	// 描画
	void Render(GameCamera& camera);
	// 更新
	void Update();
};

