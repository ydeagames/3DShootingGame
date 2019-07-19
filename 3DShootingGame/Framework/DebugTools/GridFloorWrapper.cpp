#include "pch.h"
#include "GridFloorWrapper.h"
#include <Framework/GameContext.h>
#include <Framework/GameCamera.h>

void GridFloorWrapper::Initialize(GameContext & context)
{
	// グリッド床作成
	m_pGridFloor = std::make_unique<GridFloor>(context.GetDR().GetD3DDevice(), context.GetDR().GetD3DDeviceContext(), &context.GetStates(), 10.0f, 10);
}

void GridFloorWrapper::Render(GameContext & context)
{
	m_pGridFloor->draw(context.GetDR().GetD3DDeviceContext(), context.GetCamera().view, context.GetCamera().projection);
}
