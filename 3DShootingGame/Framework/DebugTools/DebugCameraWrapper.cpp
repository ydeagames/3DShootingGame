#include "pch.h"
#include "DebugCameraWrapper.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>

DebugCameraWrapper::DebugCameraWrapper()
{
	// グリッド床作成
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
