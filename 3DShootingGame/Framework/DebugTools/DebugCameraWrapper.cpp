#include "pch.h"
#include "DebugCameraWrapper.h"
#include <Framework/GameContext.h>
#include <Framework/GameCamera.h>

DebugCameraWrapper::DebugCameraWrapper()
{
	// グリッド床作成
	m_pDebugCamera = std::make_unique<DebugCamera>();
}

DebugCamera & DebugCameraWrapper::GetDebugCamera()
{
	return *m_pDebugCamera;
}

void DebugCameraWrapper::Initialize(GameContext & context)
{
}

void DebugCameraWrapper::Update(GameContext & context)
{
	m_pDebugCamera->update();
	context.GetCamera().view = m_pDebugCamera->getViewMatrix();
}
