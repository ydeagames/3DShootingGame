// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 14:22:32 +0900
// Modified: 2020-01-06 14:19:53 +0900

#include "pch.h"
#include "Background.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/WindowHandler.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Background::RenderStart()
{
	auto& dr = GameContext::Get<DX::DeviceResources>();
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	std::vector<GeometricPrimitive::VertexType> vertices = {
		{ Vector3(-0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 0.0f) },
		{ Vector3(+0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 0.0f) },
		{ Vector3(+0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 1.0f) },
	};
	std::vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3,
	};
	m_plane = GeometricPrimitive::CreateCustom(dr.GetD3DDeviceContext(), vertices, indices);

	std::wstring name = cv.from_bytes(texture);
	if (FAILED(CreateWICTextureFromFile(
		dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
		name.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
		m_texture = nullptr;
}

void Background::Render(GameCamera& camera)
{
	if (m_texture)
		if (fullScreen)
		{
			m_plane->Draw(gameObject.GetComponent<Transform>().GetMatrix() * Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::White, m_texture.Get());
		}
		else if (uiSpace)
		{
			auto s = GameContext::Get<WindowHandler>().GetSize();
			m_plane->Draw(gameObject.GetComponent<Transform>().GetMatrix() * Matrix::CreateScale(2 / s.x, 2 / s.y, 1), Matrix::Identity, Matrix::Identity, Colors::White, m_texture.Get());
		}
		else
		{
			m_plane->Draw(gameObject.GetComponent<Transform>().GetMatrix(), camera.view, camera.projection, Colors::White, m_texture.Get());
		}
}

void Background::EditorGui()
{
	std::string tmpname = texture;
	tmpname.resize(128);
	if (ImGui::InputText("Texture", &tmpname[0], tmpname.size()))
	{
		texture = std::string(tmpname.c_str());
		RenderStart();
	}

	ImGui::Checkbox("Full Screen", &fullScreen);
	ImGui::Checkbox("UI Space", &uiSpace);
}


