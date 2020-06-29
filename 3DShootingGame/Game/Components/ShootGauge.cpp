// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-20 08:46:50 +0900
// Modified: 2020-01-27 14:06:31 +0900

#include "pch.h"
#include "ShootGauge.h"
#include <Framework/Components/Transform.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include "Utilities/StringCast.h"
#include "Utilities/MathUtils.h"
#include "Framework/ImGui/WidgetDND.h"
#include "PlayerController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void ShootGauge::Update()
{
	time += float(GameContext::Get<DX::StepTimer>().GetElapsedSeconds()) * speed;
	percent = 1 - std::abs(std::sin(time * 2));
	if (gameObject.registry->valid(player))
	{
		auto playerObj = gameObject.Wrap(player);
		if (playerObj.HasComponent<PlayerController>())
		{
			auto& playerCtrl = playerObj.GetComponent<PlayerController>();
			if (playerCtrl.powerTime > 0)
			{
				time = playerCtrl.powerTime;
				percent = playerCtrl.percent;
			}
			else
			{
				time = 0;
				percent = 0;
			}
		}
	}
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
	{
		auto& reg = *gameObject.registry;
		auto& e = player;
		int iid = (e == entt::null) ? -1 : int(reg.entity(e));
		if (ImGui::InputInt("Player", &iid))
			if (iid < 0)
				e = entt::null;
			else
			{
				auto id = entt::entity(iid);
				e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;
			}

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto* data = Widgets::WidgetDND::AcceptDragDropPayload())
			{
				if (data->regptr == &reg)
					e = data->entity;
			}
			ImGui::EndDragDropTarget();
		}
	}
}


