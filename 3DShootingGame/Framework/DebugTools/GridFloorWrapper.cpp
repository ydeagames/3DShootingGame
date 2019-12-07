#include "pch.h"
#include "GridFloorWrapper.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>

void GridFloorWrapper::RenderInitialize()
{
	// グリッド床作成
	m_pGridFloor = std::make_unique<GridFloor>(GameContext::Get<DX::DeviceResources>().GetD3DDevice(), GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext(), &GameContext::Get<DirectX::CommonStates>(), 10.0f, 10);
}

void GridFloorWrapper::Render(GameCamera& camera)
{
	m_pGridFloor->draw(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext(), camera.view, camera.projection);
}
