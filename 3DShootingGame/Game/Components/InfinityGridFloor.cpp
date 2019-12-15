#include "pch.h"
#include "InfinityGridFloor.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>

// 描画初期化
void InfinityGridFloor::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));
}

// 描画
void InfinityGridFloor::Render(GameCamera& camera)
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	auto& states = GameContext::Get<DirectX::CommonStates>();

	context->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);

	m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
	m_basicEffect->SetView(camera.view);
	m_basicEffect->SetProjection(camera.projection);


	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	auto campos = camera.GetPosition();
	auto ray1 = camera.ViewportPointToRay(DirectX::SimpleMath::Vector3(-1, -1, 0));
	auto ray2 = camera.ViewportPointToRay(DirectX::SimpleMath::Vector3(1, 1, 0));
	DirectX::SimpleMath::Plane plane = DirectX::SimpleMath::Plane(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Up);

	DirectX::SimpleMath::Vector3 q1 = campos + DirectX::SimpleMath::Vector3(size.x, 0, size.y) * .5f;
	DirectX::SimpleMath::Vector3 q2 = campos + DirectX::SimpleMath::Vector3(size.x, 0, size.y) * -.5f;

	m_primitiveBatch->Begin();

	float ax1 = std::min(q1.x, q2.x);
	float ax2 = std::max(q1.x, q2.x);
	float az1 = std::min(q1.z, q2.z);
	float az2 = std::max(q1.z, q2.z);
	int tx1 = int(std::floor(ax1 / cellsize));
	int tx2 = int(std::floor(ax2 / cellsize));
	int tz1 = int(std::floor(az1 / cellsize));
	int tz2 = int(std::floor(az2 / cellsize));
	for (int i = tx1; i <= tx2; ++i)
	{
		DirectX::VertexPositionColor v1(DirectX::SimpleMath::Vector3(i * cellsize, 0, az1), color);
		DirectX::VertexPositionColor v2(DirectX::SimpleMath::Vector3(i * cellsize, 0, az2), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}
	for (int i = tz1; i <= tz2; ++i)
	{
		DirectX::VertexPositionColor v1(DirectX::SimpleMath::Vector3(ax1, 0, i * cellsize), color);
		DirectX::VertexPositionColor v2(DirectX::SimpleMath::Vector3(ax2, 0, i * cellsize), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}

void InfinityGridFloor::EditorGui()
{
	ImGui::DragFloat("Cell Size", &cellsize);
	ImGui::DragFloat2("Size", &size.x);
	ImGui::ColorEdit4("Color", color.m128_f32);
}

