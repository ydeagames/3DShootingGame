// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-12-08 01:27:19 +0900

#include "pch.h"
#include "DebugCameraWrapper.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>

DebugCameraWrapper::DebugCameraWrapper()
{
	// ÉOÉäÉbÉhè∞çÏê¨
	m_pDebugCamera = std::make_unique<DebugCamera>();
}

DebugCamera & DebugCameraWrapper::GetDebugCamera()
{
	return *m_pDebugCamera;
}

void DebugCameraWrapper::Initialize()
{
}

void DebugCameraWrapper::Render(GameCamera& camera)
{
	m_camera = &camera;
}

void DebugCameraWrapper::Update()
{
	m_pDebugCamera->update();
	if (m_camera)
		m_camera->view = m_pDebugCamera->getViewMatrix();
}


