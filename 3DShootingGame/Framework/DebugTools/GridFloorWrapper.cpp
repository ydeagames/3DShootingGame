// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-13 16:28:17 +0900

#include "pch.h"
#include "GridFloorWrapper.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>

void GridFloorWrapper::RenderStart()
{
	// ÉOÉäÉbÉhè∞çÏê¨
	m_pGridFloor = std::make_unique<GridFloor>(GameContext::Get<DX::DeviceResources>().GetD3DDevice(), GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext(), &GameContext::Get<DirectX::CommonStates>(), 10.0f, 10);
}

void GridFloorWrapper::Render(GameCamera& camera)
{
	m_pGridFloor->draw(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext(), camera.view, camera.projection);
}


