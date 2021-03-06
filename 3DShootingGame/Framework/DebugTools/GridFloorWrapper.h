// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-13 16:28:17 +0900

#include <Framework/ECS/Component.h>
#include "GridFloor.h"

class GameCamera;

class GridFloorWrapper : public Component
{
	// グリッド床
	std::unique_ptr<GridFloor> m_pGridFloor;
	// 生成
	void RenderStart();
	// 描画
	void Render(GameCamera& camera);
};


