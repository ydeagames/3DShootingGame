#include "pch.h"
#include "ShootGauge.h"
#include <Framework/Components/Transform.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include "Utilities/StringCast.h"
#include "Utilities/MathUtils.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void ShootGauge::Update()
{
	time += float(GameContext::Get<DX::StepTimer>().GetElapsedSeconds()) * speed;
	percent = 1 - std::abs(std::sin(time * 2));
}

void ShootGauge::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetTextureEnabled(true);
	// プリミティブバッチ
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));

	const auto texture_str = string_cast<std::wstring>(texture);
	// テクスチャ
	if (FAILED(CreateWICTextureFromFile(
		device, context,
		texture_str.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
		m_texture = nullptr;
}

void ShootGauge::RenderGui(GameCamera& camera)
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	auto& states = GameContext::Get<DirectX::CommonStates>();

	context->OMSetBlendState(states.NonPremultiplied(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);

	m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
	m_basicEffect->SetView(Matrix::Identity);
	m_basicEffect->SetProjection(Matrix::Identity);

	if (m_texture)
	{
		m_basicEffect->SetTexture(m_texture.Get());
		
		m_basicEffect->Apply(context);
		context->IASetInputLayout(m_pInputLayout.Get());

		std::vector<VertexPositionTexture> vertices = {
			{ Vector3(-0.5f, MathUtils::Lerp(percent, -0.5f, +0.5f), 0.0f), Vector2(0.0f, MathUtils::Lerp(percent, 1.0f, 0.0f)) },
			{ Vector3(+0.5f, MathUtils::Lerp(percent, -0.5f, +0.5f), 0.0f), Vector2(1.0f, MathUtils::Lerp(percent, 1.0f, 0.0f)) },
			{ Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0f, 1.0f) },
		};
		std::vector<uint16_t> indices = {
			0, 1, 2, 0, 2, 3,
		};
		m_primitiveBatch->Begin();
		m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices.data(), indices.size(), vertices.data(), vertices.size());
		m_primitiveBatch->End();
	}
}

void ShootGauge::EditorGui()
{
	{
		std::string tmpname = texture;
		tmpname.resize(128);
		if (ImGui::InputText("Texture", &tmpname[0], tmpname.size()))
		{
			texture = std::string(tmpname.c_str());
			RenderStart();
		}
	}
	ImGui::DragFloat("Speed", &speed, .01f);
	ImGui::DragFloat("Time", &time, .01f);
	ImGui::DragFloat("Percent", &percent, .01f, 0, 1);
}
