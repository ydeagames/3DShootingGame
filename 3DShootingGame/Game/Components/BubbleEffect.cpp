// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-15 16:08:06 +0900
// Modified: 2020-01-15 16:08:06 +0900

#include "pch.h"
#include "BubbleEffect.h"
#include <Framework/Components/Transform.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include "Utilities/StringCast.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void BubbleEffect::Update()
{
	time += float(GameContext::Get<DX::StepTimer>().GetElapsedSeconds());
}

void BubbleEffect::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// スプライトバッチ
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));

	auto texture_str = string_cast<std::wstring>(texture);
	// テクスチャ
	if (FAILED(CreateWICTextureFromFile(
		device, context,
		texture_str.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
		m_texture = nullptr;
}

void BubbleEffect::Render(GameCamera& camera)
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

	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
	if (m_texture)
	{
		std::mt19937 rand(seed);
		const std::uniform_real_distribution<float> dist(0, 1);
		const std::uniform_real_distribution<float> dist2(.25f, .75f);
		const std::uniform_real_distribution<float> dist3(.25f, 6.75f);
		const auto size = camera.GetWindowSize();
		
		for (int i = 0; i < count; i++)
		{
			const auto rx = dist(rand);
			const auto ry = dist(rand);
			const auto y0 = std::fmod(ry + time * dist2(rand), 1.f);
			const auto y = ((1 - y0) * 1.5f - 0.25f);
			const auto rot = dist2(rand) * time * dist3(rand);
			const auto scale = dist2(rand);
			m_spriteBatch->Draw(m_texture.Get(),
				Vector2(rx * size.x, y * size.y),
				nullptr,
				Color(1, 1, 1, (1 - y0) * (1 - y0)),
				rot,
				Vector2(128, 128),
				scale);
		}
	}
	m_spriteBatch->End();
}

void BubbleEffect::EditorGui()
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
	ImGui::DragFloat("Time", &time);
	ImGui::DragInt("Count", &count);
	ImGui::DragInt("Seed", &seed);
}


